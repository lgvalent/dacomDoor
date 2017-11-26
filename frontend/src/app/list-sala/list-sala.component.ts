import { Router } from '@angular/router';
import { Component, ElementRef, OnInit, ViewChild } from '@angular/core';

import { ModalSalaComponent } from './modal-sala/modal-sala.component';
import { ModalHorarioComponent } from './modal-horario/modal-horario.component';

import { DialogService } from 'ng2-bootstrap-modal';
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
  options = ['Dia'];

  constructor(
    private dbService: DatabaseService,
    private dialogService: DialogService,
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
    const disposable = this.dialogService
      .addDialog(ModalSalaComponent, {
        title: 'Sala - ' + mode,
        sala: id >= 0 ? this.salas[id] : {},
        salas: this.salas,
        mode: mode
      })
      .subscribe(isConfirmed => {});
  }

  novoHorario() {
    if (this.dbService.checkToken()) {
      localStorage.removeItem('token');
      this.router
        .navigate([''])
        .then(() => alert('Sua sessão expirou, logue novamente!'));
      return;
    }
    const Disposable = this.dialogService
      .addDialog(ModalHorarioComponent, {
        tipo: 'novo',
        title: 'Horário - Cadastro',
        buttonText: 'Cadastrar',
        salaId: this.salas[this.id].id,
        horarios: this.salas[this.id].horarios
      })
      .subscribe(isConfirmed => {});
  }

  editarHorario(id) {
    if (this.dbService.checkToken()) {
      localStorage.removeItem('token');
      this.router
        .navigate([''])
        .then(() => alert('Sua sessão expirou, logue novamente!'));
      return;
    }
    const Disposable = this.dialogService
      .addDialog(ModalHorarioComponent, {
        tipo: 'editar',
        title: 'Horário - Editar',
        buttonText: 'Editar',
        salaId: this.salas[this.id].id,
        horarios: this.salas[this.id].horarios,
        index: id
      })
      .subscribe(isConfirmed => {});
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
