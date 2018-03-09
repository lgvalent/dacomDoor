## Módulo leitura do cartão

O módulo é responsável por monitorar o sensor, validar o cartão, abrir a porta e registrar o evento no banco

### Requisitos
- Python2 `python`
- Gerenciador de pacotes pip `python-pip`
- Banco de dados MySQL `mysql-server`


## Para executar
- Instalar as dependências com o comando `pip install -r requirements.txt` (é necessário fazer isso somente uma vez).
- Alterar as configurações do banco em `config.py`
- Executar `python run.py` (use o Postman para testar os métodos)
