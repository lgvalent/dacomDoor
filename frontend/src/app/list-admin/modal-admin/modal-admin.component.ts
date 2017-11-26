import { Router } from '@angular/router';
import { Component, OnInit } from '@angular/core';

import { BsModalRef } from 'ngx-bootstrap/modal/bs-modal-ref.service';

import { DatabaseService } from './../../servicos/database.service';

@Component({
  selector: 'app-modal-admin',
  templateUrl: './modal-admin.component.html',
  styleUrls: ['./modal-admin.component.css']
})
export class ModalAdminComponent implements OnInit {
  mode: string;
  admin?: any;
  admins: any;
  index?: any;

  constructor(
    private dbService: DatabaseService,
    public bsModalRef: BsModalRef,
    private router: Router
  ) {}

  ngOnInit() {}

  private kickUser() {
    localStorage.removeItem('token');
    this.router
      .navigate([''])
      .then(() => this.bsModalRef.hide())
      .then(() => alert('Sua sessão expirou, logue novamente!'));
  }

  getAttribute(attr) {
    return this.admin !== undefined ? this.admin[attr] : '';
  }

  onSubmit(form) {
    if (form.valid) {
      const { value } = form;
      delete value.id;

      if (this.mode === 'Editar') {
        const request = this.dbService.editarRecurso(
          'admins',
          this.admin.id,
          value
        );
        if (request) {
          request
            .then(res => {
              this.admins[this.index] = res;
              this.bsModalRef.hide();
            })
            .catch(err => this.handleError(err.status));
        } else {
          this.kickUser();
        }
      } else if (this.mode === 'Cadastrar') {
        const request = this.dbService.criarRecurso('admins', value);
        if (request) {
          request
            .then(res => {
              this.admins.push(res);
              this.bsModalRef.hide();
            })
            .catch(err => this.handleError(err.status));
        } else {
          this.kickUser();
        }
      }
    }
  }

  private handleError(error: number) {
    if (error === 409) {
      alert('Este administrador já existe!');
    } else if (error === 400) {
      alert('Ops, há algo errado nesta página ou configurações do servidor');
    } else if (error === 401) {
      this.bsModalRef.hide();
      localStorage.removeItem('token');
      this.router.navigate(['']).then(() => {
        alert('Credenciais inválidas');
      });
    } else if (error === 404) {
      alert('Este administrador não existe!');
    } else if (error === 0) {
      alert('Erro de conexão, tente novamente!');
    }
  }
}
