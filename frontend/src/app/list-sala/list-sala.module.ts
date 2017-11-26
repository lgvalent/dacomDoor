import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { CommonModule } from '@angular/common';

import { HomeModule } from '../home/home.module';
import { TooltipModule } from 'ngx-bootstrap/tooltip';
import { TimepickerModule } from 'ngx-bootstrap/timepicker';

import { ListSalaComponent } from './list-sala.component';
import { ModalSalaComponent } from './modal-sala/modal-sala.component';
import { ModalHorarioComponent } from './modal-horario/modal-horario.component';

@NgModule({
  imports: [
    CommonModule,
    FormsModule,
    HomeModule,
    TooltipModule.forRoot(),
    TimepickerModule.forRoot()
  ],
  declarations: [ListSalaComponent, ModalSalaComponent, ModalHorarioComponent],
  exports: [ListSalaComponent, ModalSalaComponent, ModalHorarioComponent],
  entryComponents: [ModalHorarioComponent, ModalSalaComponent]
})
export class ListSalaModule {}
