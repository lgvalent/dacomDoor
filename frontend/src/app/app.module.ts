import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule } from '@angular/forms';
import { RouterModule, Routes } from '@angular/router';
import { AppComponent } from './app.component';
import { HttpClientModule } from '@angular/common/http';
import { HttpModule } from '@angular/http';

import { JwtHelper } from 'angular2-jwt';
import { BootstrapModalModule } from 'ng2-bootstrap-modal';
import { AngularMultiSelectModule } from 'angular2-multiselect-dropdown/angular2-multiselect-dropdown';

import { routing } from './app.routing';

import { LoginModule } from './login/login.module';
import { HomeModule } from './home/home.module';
import { ListSalaModule } from './list-sala/list-sala.module';
import { ListUserModule } from './list-user/list-user.module';
import { ListAdminModule } from './list-admin/list-admin.module';

import { PaginaErroComponent } from './pagina-erro/pagina-erro.component';

import { LoginService } from './servicos/login.service';
import { DatabaseService } from './servicos/database.service';
import { AuthGuardService } from './servicos/auth-guard.service';
import { LoginDisabledService } from './servicos/login-disabled.service';

@NgModule({
  declarations: [AppComponent, PaginaErroComponent],
  imports: [
    routing,
    HttpModule,
    FormsModule,
    BrowserModule,
    HttpClientModule,
    HomeModule,
    LoginModule,
    ListUserModule,
    ListSalaModule,
    ListAdminModule,
    BootstrapModalModule.forRoot({ container: document.body })
  ],
  providers: [
    JwtHelper,
    DatabaseService,
    AuthGuardService,
    LoginDisabledService
  ],
  bootstrap: [AppComponent]
})
export class AppModule {}
