import { Router } from '@angular/router';
import { FormsModule } from '@angular/forms';
import { DatabaseService } from './../../servicos/database.service';
import { Component, OnInit, ViewEncapsulation } from '@angular/core';
import { DialogComponent, DialogService } from 'ng2-bootstrap-modal';

export interface ConfirmModel {
  title: string;
  sala: any;
  salas: any[];
  mode: string;
}

@Component({
  selector: 'app-modal-sala',
  templateUrl: './modal-sala.component.html',
  styleUrls: ['./modal-sala.component.css'],
  encapsulation: ViewEncapsulation.None
})
export class ModalSalaComponent extends DialogComponent<ConfirmModel, boolean>
  implements ConfirmModel, OnInit {
  title: string;
  sala: any;
  salas: any[];
  mode: string;

  constructor(
    dialogService: DialogService,
    private dbService: DatabaseService,
    private router: Router
  ) {
    super(dialogService);
  }

  ngOnInit() {}

  private kickUser() {
    localStorage.removeItem('token');
    this.router
      .navigate([''])
      .then(() => this.close())
      .then(() => alert('Sua sessão expirou, logue novamente!'));
  }

  onSubmit(formModalSala) {
    if (formModalSala.valid) {
      this.sala.nome = formModalSala.value.nome_sala;
      this.alterarSala(this.sala);
    }
  }

  alterarSala(sala) {
    if (this.mode === 'Editar') {
      const request = this.dbService.editarRecurso('salas', sala.id, {
        nome: this.sala.nome
      });
      if (request) {
        request
          .then(res => {
            alert('Sala alterada com sucesso!');
            this.close();
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
            this.close();
          })
          .catch(err => this.handleError(err.status));
      } else {
        this.kickUser();
      }
    }
  }

  private handleError(error: number) {
    if (error === 409) {
      alert('Esta sala já existe!');
    } else if (error === 400) {
      alert('Ops, há algo errado nesta página ou configurações do servidor');
    } else if (error === 401) {
      this.close();
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
