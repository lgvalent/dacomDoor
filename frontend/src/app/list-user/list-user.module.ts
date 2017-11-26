import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { CommonModule } from '@angular/common';

import { HomeModule } from '../home/home.module';

import { TooltipModule } from 'ngx-bootstrap';
import { BsDropdownModule } from 'ngx-bootstrap/dropdown';
import { MultiselectDropdownModule } from 'angular-2-dropdown-multiselect';

import { ListUserComponent } from './list-user.component';
import { ModalUserComponent } from './modal-user/modal-user.component';

@NgModule({
  imports: [
    CommonModule,
    FormsModule,
    HomeModule,
    MultiselectDropdownModule,
    BsDropdownModule.forRoot(),
    TooltipModule.forRoot()
  ],
  declarations: [ListUserComponent, ModalUserComponent],
  exports: [ListUserComponent, ModalUserComponent],
  entryComponents: [ModalUserComponent]
})
export class ListUserModule {}
