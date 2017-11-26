import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { routing } from '../app.routing';

import { HomeComponent } from './home.component';
import { BsDropdownModule } from 'ngx-bootstrap/dropdown';

@NgModule({
  imports: [
    routing,
    CommonModule,
    BsDropdownModule.forRoot()
  ],
  declarations: [HomeComponent],
  exports: [HomeComponent]
})
export class HomeModule { }
