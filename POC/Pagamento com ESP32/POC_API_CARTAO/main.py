import pandas as pd
from flask import Flask, request, jsonify

app = Flask(__name__)


# Rota para verificar se a API está no ar
@app.route('/')
def homepage():
  return 'A API está no ar'


# Rota para obter o saldo do usuário com base no nome de usuário
@app.route('/saldo', methods=['POST'])
def obter_saldo():
  # Recebe os dados do formulário
  dados_formulario = request.form
  nome_usuario = dados_formulario.get('nome_usuario')

  # Lê o arquivo CSV com as contas
  df = pd.read_csv('ACCOUNTS.csv', sep=';')

  # Filtra o DataFrame para obter o saldo do usuário com base no nome de usuário
  usuario = df[df['USER'] == nome_usuario]

  # Verifica se o usuário foi encontrado
  if not usuario.empty:
    saldo_usuario = usuario['SALDO'].iloc[0]
    response = {"nome_usuario": nome_usuario, "saldo": f"{saldo_usuario}"}
  else:
    response = {"error": nome_usuario}

  return jsonify(response)


if __name__ == '__main__':
  app.run(host='0.0.0.0')
