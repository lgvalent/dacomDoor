## API RESTful

API RESTful para gerenciar acesso de usuários aos laboratórios da UTFPR-CM.

### Requisitos
- Python3
- Gerenciador de pacotes Python: pip3
- Banco de dados MySQL
- Alterar as configurações do banco em `config.py`
- Após configurar o banco de dados, executar as migrações: `python3 migrate.py db init`, `python3 migrate.py db migrate` e `python3 migrate.py db upgrade` 


## Para executar
- Instalar as dependências com o comando `pip3 install -r requirements.txt` (é necessário fazer isso somente uma vez).
- `python3 run.py` (usem o Postman para testar os métodos)
