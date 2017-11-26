import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { CommonModule } from '@angular/common';

import { LoginService } from '../servicos/login.service';
import { LoginComponent } from './login.component';

@NgModule({
  imports: [CommonModule, FormsModule],
  declarations: [LoginComponent],
  exports: [LoginComponent],
  providers: [LoginService]
})
export class LoginModule {}
