import { Injectable } from '@angular/core';
import { Router, CanActivate } from '@angular/router';
import { JwtHelper } from 'angular2-jwt';

@Injectable()
export class LoginDisabledService implements CanActivate {
  constructor(private router: Router, private jwtHelper: JwtHelper) {}

  canActivate() {
    const token = localStorage.getItem('token');
    const flag = localStorage.getItem('flag');
    if (token) {
      this.router.navigate(['home']);
      return false;
    }
    return true;
  }
}
