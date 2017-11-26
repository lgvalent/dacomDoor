import { Router } from '@angular/router';
import { FormsModule } from '@angular/forms';
import { Component, OnInit } from '@angular/core';

import { BsModalRef } from 'ngx-bootstrap/modal/bs-modal-ref.service';

import { DatabaseService } from './../../servicos/database.service';

@Component({
  selector: 'app-modal-sala',
  templateUrl: './modal-sala.component.html',
  styleUrls: ['./modal-sala.component.css']
})
export class ModalSalaComponent implements OnInit {
  title: string;
  sala: any;
  salas: any[];
  mode: string;

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
    return this.sala !== undefined ? this.sala[attr] : '';
  }

  onSubmit(formModalSala) {
    if (formModalSala.valid) {
      this.sala.nome = formModalSala.value.nome_sala;

      if (this.mode === 'Editar') {
        const request = this.dbService.editarRecurso('salas', this.sala.id, {
          nome: this.sala.nome
        });
        if (request) {
          request
            .then(res => {
              alert('Sala alterada com sucesso!');
              this.bsModalRef.hide();
            })
            .catch(err => this.handleError(err.status));
        } else {
          this.kickUser();
        }
      } else {
        const request = this.dbService.criarRecurso('salas', {
          nome: this.sala.nome
        });
        if (request) {
          request
            .then(res => {
              alert('Sala criada com sucesso!');
              this.salas.push(res);
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
      alert('Esta sala já existe!');
    } else if (error === 400) {
      alert('Ops, há algo errado nesta página ou configurações do servidor');
    } else if (error === 401) {
      this.bsModalRef.hide();
      localStorage.removeItem('token');
      this.router.navigate(['']).then(() => {
        alert('Credenciais inválidas');
      });
    } else if (error === 404) {
      alert('Esta sala não existe!');
    } else if (error === 0) {
      alert('Erro de conexão, tente novamente!');
    }
  }
}
