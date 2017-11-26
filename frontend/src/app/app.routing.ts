import { ListAdminComponent } from './list-admin/list-admin.component';
import { Routes, RouterModule } from '@angular/router';

import { HomeComponent } from './home/home.component';
import { LoginComponent } from './login/login.component';
import { ListSalaComponent } from './list-sala/list-sala.component';
import { ListUserComponent } from './list-user/list-user.component';
import { PaginaErroComponent } from './pagina-erro/pagina-erro.component';

import { AuthGuardService } from './servicos/auth-guard.service';
import { ModuleWithProviders } from '@angular/core';
import { LoginDisabledService } from './servicos/login-disabled.service';

const APP_ROUTERS: Routes = [
  { path: 'home', component: HomeComponent, canActivate: [AuthGuardService] },
  {
    path: 'cadastro-adm',
    component: ListAdminComponent,
    canActivate: [AuthGuardService]
  },
  {
    path: 'cadastro-sala',
    component: ListSalaComponent,
    canActivate: [AuthGuardService]
  },
  {
    path: 'cadastro-usuario',
    component: ListUserComponent,
    canActivate: [AuthGuardService]
  },
  { path: '', component: LoginComponent, canActivate: [LoginDisabledService] },
  {
    path: '**',
    component: PaginaErroComponent,
    canActivate: [AuthGuardService]
  }
];

export const routing: ModuleWithProviders = RouterModule.forRoot(APP_ROUTERS);
