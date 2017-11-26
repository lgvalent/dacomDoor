import { Router } from '@angular/router';
import { Component, ElementRef, OnInit, ViewChild } from '@angular/core';

import { ModalSalaComponent } from './modal-sala/modal-sala.component';
import { ModalHorarioComponent } from './modal-horario/modal-horario.component';

import { BsModalService } from 'ngx-bootstrap/modal';
import { BsModalRef } from 'ngx-bootstrap/modal/bs-modal-ref.service';

import { DatabaseService } from '../servicos/database.service';

@Component({
  selector: 'app-list-sala',
  templateUrl: './list-sala.component.html',
  styleUrls: ['./list-sala.component.css']
})
export class ListSalaComponent implements OnInit {
  @ViewChild('button') btn: ElementRef;
  id = -1;
  salas: any = [];
  bsModalRef: BsModalRef;

  constructor(
    private dbService: DatabaseService,
    private modalService: BsModalService,
    private router: Router
  ) {}

  ngOnInit() {
    this.dbService
      .getRecurso('salas')
      .then(data => (this.salas = data))
      .catch(err => this.handleError('Sala', err.status));
  }

  onclick(idx) {
    const classes = this.btn.nativeElement.children[idx].className.split(' ');
    if (classes.indexOf('active') === -1) {
      this.btn.nativeElement.children[idx].className += ' active';
      for (let i = 0; i < this.btn.nativeElement.children.length; i++) {
        if (i !== idx) {
          this.btn.nativeElement.children[i].className =
            'btn list-group-item li_sala';
        }
      }
    }
    if (idx === this.id) {
      this.btn.nativeElement.children[idx].className =
        'btn list-group-item li_sala';
      this.id = -1;
    } else {
      this.id = idx;
    }
  }

  deletarSala(index) {
    if (confirm('Você realmente deseja apagar esta sala?')) {
      this.dbService
        .deletarRecurso('salas', this.salas[index].id)
        .then(res => {
          alert('Sala excluída com sucesso!');
          this.salas.splice(index, 1);
          this.id = -1;
        })
        .catch(err => this.handleError('Sala', err.status));
    }
  }

  showModal(id, mode = 'Editar') {
    if (this.dbService.checkToken()) {
      localStorage.removeItem('token');
      this.router
        .navigate([''])
        .then(() => alert('Sua sessão expirou, logue novamente!'));
      return;
    }

    this.bsModalRef = this.modalService.show(ModalSalaComponent);
    this.bsModalRef.content.title = 'Sala - ' + mode;
    this.bsModalRef.content.sala = id >= 0 ? this.salas[id] : {};
    this.bsModalRef.content.salas = this.salas;
    this.bsModalRef.content.mode = mode;
  }

  showModalHorario(index, mode = 'Editar') {
    if (this.dbService.checkToken()) {
      localStorage.removeItem('token');
      this.router
        .navigate([''])
        .then(() => alert('Sua sessão expirou, logue novamente!'));
      return;
    }

    this.bsModalRef = this.modalService.show(ModalHorarioComponent);
    this.bsModalRef.content.mode = mode;
    this.bsModalRef.content.title = 'Horário - ' + mode,
    this.bsModalRef.content.salaId = this.salas[this.id].id,
    this.bsModalRef.content.horarios = this.salas[this.id].horarios,
    this.bsModalRef.content.index = index;
  }

  deletarHorario(id, id_horario) {
    if (confirm('Você realmente deseja apagar este horário?')) {
      this.dbService
        .deletarRecurso('horarios', id_horario)
        .then(res => {
          alert('Horário excluído com sucesso!');
          this.salas[this.id].horarios.splice(id, 1);
        })
        .catch(err => this.handleError('Horário', err.status));
    }
  }

  deletarTodos(recurso, id = -1) {
    if (confirm('Deseja realmente apagar todos os horários ?')) {
      this.dbService
        .deletarRecurso('salas/horarios', this.salas[id].id)
        .then(res => {
          alert('Todos horários excluídos com sucesso!');
          this.salas[id].horarios = [];
        })
        .catch(err => this.handleError('Horário', err.status));
    }
  }

  private handleError(resource: string, error: number) {
    const sufix = resource === 'Sala' ? 'a' : 'e';

    if (error === 409) {
      alert(`Est${sufix} ${resource.toLowerCase()} já existe!`);
    } else if (error === 400) {
      alert('Ops, há algo errado nesta página ou configurações do servidor');
    } else if (error === 401) {
      localStorage.removeItem('token');
      this.router.navigate(['']).then(() => {
        alert('Credenciais inválidas');
      });
    } else if (error === 404) {
      alert(`Est${sufix} ${resource.toLowerCase()} não existe!`);
    } else if (error === 0) {
      alert('Erro de conexão, tente novamente!');
    }
  }
}
