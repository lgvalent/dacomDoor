import { Router } from '@angular/router';
import { JwtHelper } from 'angular2-jwt';
import { Component, OnInit } from '@angular/core';
import { DialogService } from 'ng2-bootstrap-modal/dist';

import { DatabaseService } from '../servicos/database.service';
import { ModalAdminComponent } from './modal-admin/modal-admin.component';

@Component({
  selector: 'app-list-admin',
  templateUrl: './list-admin.component.html',
  styleUrls: ['./list-admin.component.css']
})
export class ListAdminComponent implements OnInit {
  alertCtrl = {
    isHide: true,
    type: 'success',
    message: ''
  };

  admins = [];
  constructor(
    private dbService: DatabaseService,
    private dialogService: DialogService,
    private jwtHelper: JwtHelper,
    private router: Router
  ) {}

  ngOnInit() {
    this.dbService
      .getRecurso('admins')
      .then(res => (this.admins = res))
      .catch(err => this.handleError(err.status));
  }

  showAlert(type, message) {
    this.alertCtrl.isHide = false;
    this.alertCtrl.message = message;
    this.alertCtrl.type = type;
  }

  showModal(index, mode = 'Editar') {
    const disposable = this.dialogService
      .addDialog(ModalAdminComponent, {
        admin: index >= 0 ? this.admins[index] : {},
        admins: this.admins,
        index: index,
        mode: mode
      })
      .subscribe(response => {
        if (response) {
          this.showAlert('success', response.message);
        }
      });
  }

  apagarAdmin(index) {
    const { id } = this.admins[index];
    if (confirm('Você deseja apagar este administrador?')) {
      this.dbService.deletarRecurso('admins', id)
        .then(() => {
          const token = localStorage.getItem('token');
          const { identity } = this.jwtHelper.decodeToken(token);
          this.admins.splice(index, 1);
          this.showAlert('success', 'Administrador removido com sucesso!');
          if (identity === id) {
            localStorage.setItem('flag', 'MnsyBscAShMB251Sz%3');
            localStorage.removeItem('token');
          }
        })
        .catch(err => this.handleError(err.status));
    }
  }

  private handleError(error: number) {
    if (error === 400) {
      this.showAlert('danger', 'Ops, há algo errado nesta página ou configurações do servidor');
    } else if (error === 401) {
      localStorage.removeItem('token');
      this.router.navigate(['']).then(() => {
        alert('Credenciais inválidas');
      });
    } else if (error === 404) {
      this.showAlert('danger', 'Este administrador não existe!');
    } else if (error === 0) {
      this.showAlert('danger', 'Erro de conexão, tente novamente!');
    }
  }
}
