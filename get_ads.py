import json
import tqdm
import requests
import configparser
from pathlib import Path
from openpyxl import Workbook


def read_config_file(name):
    config_path = Path('config.ini')
    if config_path.is_file():
        config = configparser.ConfigParser()
        config.read(name)
        login = config['LOGIN']['Login']
        password = config['LOGIN']['Password']
        return login, password
    else:
        print('Файл конфигурации не найден')
        print('Поиск осуществляеться без учетной записи')
        print('-' * 60)
        return None, None


def get_geo_id_and_title(city_text):
    get_geo_url = ('https://api.zp.ru/v1/'
                   'geo?level=city%2Cregion%2Ccountry&q={city}')
    response = session.get(get_geo_url.format(city=city_text))
    geo_json = json.loads(response.text)
    if geo_json['geo']:
        city_id = geo_json['geo'][0]['id']
        city_title = geo_json['geo'][0]['full_title']
        return city_id, city_title
    else:
        return None, None


def get_token(login, password):
    login_and_password = {
        'login': login,
        'password': password
    }
    login_url = 'https://auth.zarplata.ru/v1/login'
    login_response = session.post(login_url, data=login_and_password)
    response_json = json.loads(login_response.text)
    auth_token = response_json['result'][0]['auth_token']
    return auth_token


def get_rubrics(rubrics_text):
    rubrics_dict = {}
    resume_url = 'https://api.zp.ru/v1/rubrics'
    response = session.get(resume_url)
    response_json = json.loads(response.text)
    rubrics_text = rubrics_text.replace(' ', '')
    rubrics_list = rubrics_text.split('+')
    for rubric in rubrics_list:
        for json_rubric in response_json['rubrics']:
            if rubric in json_rubric['title'].lower():
                rubrics_dict.update({json_rubric['title']: json_rubric['id']})
    return rubrics_dict


def get_resumes(city_id, rubrics_dict):
    resumes_list = []
    resumes_url = (
        'https://api.zp.ru/v3/resumes/?'
        'fields[]=%2A&'
        'fields[]=access_status&'
        'geo_id={geo_id}&'
        'highlight=true&'
        'limit=100'
        '&offset={offset}'
    )
    for rubric_key, rubric_id in rubrics_dict.items():
        resumes_url += '&rubric_id[]={rubric_id}'.format(rubric_id=rubric_id)
    response = session.get(resumes_url.format(geo_id=city_id, offset=0))
    response_json = json.loads(response.text)
    resumes_count = response_json['metadata']['resultset']['count']
    resumes_page_count = round((int(resumes_count)/100) + 0.5)
    if resumes_page_count > 100:
        resumes_page_count = 100
    for page in tqdm.tqdm(range(resumes_page_count)):
        response = session.get(resumes_url.format(
            geo_id=city_id,
            offset=page * 100)
        )
        response_json = json.loads(response.text)
        resumes = response_json['resumes']
        for resume in resumes:
            try:
                full_name = resume['profile']['name']['full_name']
            except TypeError:
                full_name = None
            try:
                city = resume['profile']['personal']['city']['title']
            except TypeError:
                city = None
            try:
                position = resume['position']
            except TypeError:
                position = None
            try:
                phone = resume['profile']['contact']['phone']['format']
            except TypeError:
                phone = None
            try:
                email = resume['profile']['contact']['email']
            except TypeError:
                email = None
            resumes_list.append({
                'full_name': full_name,
                'city': city,
                'position': position,
                'phone': phone,
                'email': email
                })
    return resumes_list


def fill_workbook(name, resumes):
    wb = Workbook()
    ws = wb.active
    ws.title = 'Резюме'
    head_table = [
        'ФИО',
        'Город',
        'Должность',
        'Телефон',
        'Почта'
    ]
    ws.append(head_table)
    for resume in resumes:
        if resume:
            ws.append([
                resume['full_name'],
                resume['city'],
                resume['position'],
                resume['phone'],
                resume['email']
            ])
    wb.save(name + '.xlsx')


if __name__ == '__main__':
    config_file = 'config.ini'
    lk_login, lk_password = read_config_file(config_file)
    session = requests.session()
    session.headers.update(
        {'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) '
                       'AppleWebKit/537.36 (KHTML, like Gecko) '
                       'Chrome/69.0.3497.100 Safari/537.36'
         }
    )
    city_str = input('Введите название города: ')
    geo_id, geo_title = get_geo_id_and_title(city_str)
    if geo_title and geo_id:
        print('Найден город: {geo_title}'.format(geo_title=geo_title))
        print('Найден его ID: {geo_id}'.format(geo_id=geo_id))
    else:
        print('Город не найден')
        raise SystemExit(1)
    print('-' * 60)
    rubrics_str = input('Введите название рубрик: ')
    rubrics = get_rubrics(rubrics_str)
    print('Выбраны рубрики для поиска: ')
    for key, value in rubrics.items():
        print(key)
    print('-' * 60)
    if lk_login:
        token = get_token(lk_login, lk_password)
        session.headers.update({'Authorization': 'token ' + token})
    got_resumes = get_resumes(geo_id, rubrics)
    file_name_list = [
        key.replace(', ', '_').replace(' ', '_')
        for key, value in rubrics.items()
    ]
    file_name = '_+_'.join(file_name_list)
    fill_workbook(file_name, got_resumes)
