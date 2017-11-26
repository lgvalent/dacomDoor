import { Router } from '@angular/router';
import { Component, OnInit, ViewChild, ElementRef } from '@angular/core';

import { LoginService } from '../servicos/login.service';

@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.css']
})
export class LoginComponent implements OnInit {
  bool = false;
  @ViewChild('flag') passwdInput: ElementRef;

  constructor(private router: Router, private loginService: LoginService) {}
  ngOnInit() {}

  async onSubmit(fLogin) {
    if (fLogin.valid) {
      this.bool = await this.loginService.login(
        fLogin.value.email,
        fLogin.value.password
      );
      if (this.bool) {
        this.router.navigate(['/home']);
      }
      fLogin.reset();
    }
  }

  handlePassword() {
    const inputType = this.passwdInput.nativeElement.children[0];
    const icon = this.passwdInput.nativeElement.children[1].children[0]
      .children[0];
    if (inputType.type === 'text') {
      inputType.type = 'password';
      icon.className = 'glyphicon glyphicon-eye-open';
    } else {
      inputType.type = 'text';
      icon.className = 'glyphicon glyphicon-eye-close';
    }
  }
}
