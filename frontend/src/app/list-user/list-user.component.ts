import { Router } from '@angular/router';
import { Component, OnInit } from '@angular/core';

import { BsModalService } from 'ngx-bootstrap/modal';
import { BsModalRef } from 'ngx-bootstrap/modal/bs-modal-ref.service';

import { DatabaseService } from '../servicos/database.service';
import { ModalUserComponent } from './modal-user/modal-user.component';

@Component({
  selector: 'app-list-user',
  templateUrl: './list-user.component.html',
  styleUrls: ['./list-user.component.css']
})
export class ListUserComponent implements OnInit {
  users: any = [];
  options = ['Todos', 'Nome', 'Sala', 'Tipo'];

  bsModalRef: BsModalRef;
  constructor(
    private dbService: DatabaseService,
    private modalService: BsModalService,
    private router: Router
  ) {}

  ngOnInit() {
    this.dbService
      .getRecurso('usuarios')
      .then(data => this.users = data)
      .catch(err => this.handleError(err.status));
  }

  apagarUsuario(id) {
    if (confirm('Você realmente deseja apagar este usuário?')) {
      this.dbService
        .deletarRecurso('usuarios', this.users[id].id)
        .then(res => {
          this.users.splice(id, 1);
          alert('Usuário apagado com sucesso!');
        })
        .catch(err => this.handleError(err.status));
    }
  }

  showModal(index, mode = 'Editar') {
    if (this.dbService.checkToken()) {
      localStorage.removeItem('token');
      this.router
        .navigate([''])
        .then(() => alert('Sua sessão expirou, logue novamente!'));
      return;
    }

    this.bsModalRef = this.modalService.show(ModalUserComponent);
    this.bsModalRef.content.title = 'Usuário - ' + mode;
    this.bsModalRef.content.user = index >= 0 ? this.users[index] : {};
    this.bsModalRef.content.mode = mode;
    this.bsModalRef.content.users = this.users;
    this.bsModalRef.content.index = index;
  }

  private handleError(error: number) {
    if (error === 409) {
      alert('Este usuário já existe!');
    } else if (error === 400) {
      alert('Ops, há algo errado nesta página ou configurações do servidor');
    } else if (error === 401) {
      localStorage.removeItem('token');
      this.router.navigate(['']).then(() => {
        alert('Credenciais inválidas');
      });
    } else if (error === 404) {
      alert('Este usuário não existe!');
    } else if (error === 0) {
      alert('Erro de conexão, tente novamente!');
    }
  }

  onSubmit(form) {
    const { query, filter } = form.value;
    const body = {
      query, filter
    };

    console.log(body);
    this.dbService.pesquisar(body)
    .then(res => this.users = res)
    .catch(err => {
      if (err.status === 404) {
        alert('Esta sala não existe!');
      }
    });
  }
}
