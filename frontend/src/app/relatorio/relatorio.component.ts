import { Component, OnInit } from '@angular/core';
import moment from 'moment/src/moment';
import { DatabaseService } from '../servicos/database.service';


@Component({
  selector: 'app-relatorio',
  templateUrl: './relatorio.component.html',
  styleUrls: ['./relatorio.component.css']
})
export class RelatorioComponent implements OnInit {
  options = ['Data', 'Sala', 'Usuário', 'Tipo'];
  tipos = ['Entrada', 'Saida'];
  dataInicio = '';
  dataFim = '';
  mask = [/\d/, /\d/, '-', /\d/, /\d/, '-', /\d/, /\d/, /\d/, /\d/];

  eventos = [];

  constructor(public dbService: DatabaseService) { }

  ngOnInit() {
  }

  validaData(dataInicio, dataFim) {
    if (moment(dataInicio, 'DD-MM-YYYY', true).isValid()
      && moment(dataFim, 'DD-MM-YYYY', true).isValid()) {
      dataInicio = dataInicio.split('-').reverse().join('-');
      dataFim = dataFim.split('-').reverse().join('-');

      const obj = {
        data_inicio: dataInicio,
        data_fim: dataFim
      };

      return obj;
    }
    return null;
  }

  formataHora(hora) {
    hora = hora.split(':');
    if (hora[0].length === 1) {
      hora[0] = '0' + hora[0];
    }
    return hora.join(':');
  }

  formataData(data) {
    data = data.split('/');
    if (data[0].length === 1) {
      data[0] = '0' + data[0];
    }
    return data.join('/');
  }

  onSubmit(form) {
    const {
      tipo,
      sala,
      filter,
      usuario,
      data_inicio,
      data_fim
    } = form.value;
    let body = {};

    if (filter === 'Data') {
      const res = this.validaData(data_inicio, data_fim);
      if (res !== null) {
        body = {
          filter,
          query: {
            data_inicio: res.data_inicio,
            data_fim: res.data_fim
          }
        };
      }
    } else if (filter === 'Usuário') {
      body = { filter, query: { usuario } };
    } else if (filter === 'Sala') {
      body = { filter, query: { sala } };
    } else if (filter === 'Tipo') {
      body = { filter, query: { tipo } };
    }
    console.log(body);
    this.dbService.criarRecurso('pesquisa/evento', body)
    .then(resp => {
      this.eventos = resp;
      console.log(resp);
    })
    .catch(err => this.handleError('Evento', err.status));
  }


  private handleError(resource: string, error: number) {
    if (error === 409) {
      alert(`Este ${resource.toLowerCase()} já existe!`);
    } else if (error === 400) {
      alert('Ops, há algo errado nesta página ou configurações do servidor');
    }else if (error === 404) {
      alert(`Este ${resource.toLowerCase()} não existe!`);
    } else if (error === 0) {
      alert('Erro de conexão, tente novamente!');
    }
  }
}
