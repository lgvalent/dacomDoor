## API RESTful

API RESTful para gerenciar acesso de usuários aos laboratórios da UTFPR-CM.

### Requisitos
- Python3 `python3`
- Gerenciador de pacotes pip3 `python3-pip`
- Banco de dados MySQL `mysql-server`


## Para executar
- Instalar as dependências com o comando `pip3 install -r requirements.txt` (é necessário fazer isso somente uma vez).
- Alterar as configurações do banco em `config.py`
- Após configurar o banco de dados, executar as migrações: `python3 migrate.py db init`, `python3 migrate.py db migrate` e `python3 migrate.py db upgrade` 
- Executar `python3 run.py` (use o Postman para testar os métodos)
