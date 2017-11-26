import { Router } from '@angular/router';
import { Component, OnInit } from '@angular/core';

import { BsModalRef } from 'ngx-bootstrap/modal/bs-modal-ref.service';

import { DatabaseService } from './../../servicos/database.service';

@Component({
  selector: 'app-modal-horario',
  templateUrl: './modal-horario.component.html',
  styleUrls: ['./modal-horario.component.css'],
})
export class ModalHorarioComponent implements OnInit {
  title: string;
  mode: string;
  salaId: any;
  horarios: any;
  index: number;
  horario: any;

  typeUsers = ['Aluno', 'Professor', 'Servidor'];
  dias = ['Segunda', 'Terca', 'Quarta', 'Quinta', 'Sexta', 'Sabado', 'Domingo'];
  body = {};

  myTime = new Date();
  ismeridian = false;

  constructor(
    private dbService: DatabaseService,
    public bsModalRef: BsModalRef,
    private router: Router
  ) {}

  ngOnInit() {}

  getAttribute(attr) {
    this.horario = this.index !== undefined ? this.horarios[this.index] : {};
    return this.horario !== undefined ? this.horario[attr] : '';
  }

  private kickUser() {
    localStorage.removeItem('token');
    this.router
      .navigate([''])
      .then(() => this.bsModalRef.hide())
      .then(() => alert('Sua sessão expirou, logue novamente!'));
  }

  onSubmit(formModalHora) {
    if (formModalHora.value.horarioFim < formModalHora.value.horarioInicio) {
      alert('Horário inválido, tente novamente!');
      return;
    }
    if (formModalHora.valid) {
      this.body = {
        hora_inicio: formModalHora.value.horarioInicio,
        hora_fim: formModalHora.value.horarioFim,
        dia: formModalHora.value.dia,
        id_sala: this.salaId,
        tipo_user: formModalHora.value.tipo
      };

      if (this.mode === 'Editar') {
        this.editarHorario(this.horario.id, this.body);
      } else if (this.mode === 'Cadastrar') {
        this.cadastrarHorario(this.body);
      }
    }
  }

  cadastrarHorario(body) {
    const request = this.dbService.criarRecurso('horarios', body);
    if (request) {
      request
        .then(res => {
          alert('Horário cadastrado com sucesso');
          this.horarios.push(res);
          this.bsModalRef.hide();
        })
        .catch(err => this.handleError(err.status));
    } else {
      this.kickUser();
    }
  }

  editarHorario(id, body) {
    const request = this.dbService.editarRecurso('horarios', id, body);
    if (request) {
      request
        .then(res => {
          alert('Horário alterado com sucesso!');
          this.horarios[this.index] = res;
          this.bsModalRef.hide();
        })
        .catch(err => this.handleError(err.status));
    } else {
      this.kickUser();
    }
  }

  private handleError(error: number) {
    if (error === 409) {
      alert('Este horário já existe!');
    } else if (error === 400) {
      alert('Ops, há algo errado nesta página ou configurações do servidor');
    } else if (error === 401) {
      this.bsModalRef.hide();
      localStorage.removeItem('token');
      this.router.navigate(['']).then(() => {
        alert('Credenciais inválidas');
      });
    } else if (error === 404) {
      alert('Este horário não existe!');
    } else if (error === 0) {
      alert('Erro de conexão, tente novamente!');
    }
  }
}
