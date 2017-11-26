import { Component, OnInit } from '@angular/core';
import { DialogService } from 'ng2-bootstrap-modal';

import { DatabaseService } from '../servicos/database.service';
import { ModalUserComponent } from './modal-user/modal-user.component';

import { Router } from '@angular/router';

@Component({
  selector: 'app-list-user',
  templateUrl: './list-user.component.html',
  styleUrls: ['./list-user.component.css']
})
export class ListUserComponent implements OnInit {
  users: any = [];
  options = ['Todos', 'Nome', 'Sala', 'Tipo'];

  constructor(
    private dbService: DatabaseService,
    private dialogService: DialogService,
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
          alert('Usuário Apagado com Sucesso!!');
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
    const disposable = this.dialogService
      .addDialog(ModalUserComponent, {
        title: 'Usuário - ' + mode,
        user: index >= 0 ? this.users[index] : {},
        mode: mode,
        users: this.users,
        index: index
      })
      .subscribe(isConfirmed => {});
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
