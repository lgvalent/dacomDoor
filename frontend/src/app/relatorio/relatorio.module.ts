import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { CommonModule } from '@angular/common';

import { HomeModule } from '../home/home.module';
import { TextMaskModule } from 'angular2-text-mask';
import { BsDatepickerModule } from 'ngx-bootstrap/datepicker';

import { RelatorioComponent } from './relatorio.component';

@NgModule({
  imports: [
    CommonModule,
    FormsModule,
    HomeModule,
    TextMaskModule
  ],
  declarations: [RelatorioComponent],
  exports: [RelatorioComponent],
  providers: []
})
export class RelatorioModule {}
