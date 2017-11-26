import { Router, RouterLink } from '@angular/router';
import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'app-home',
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.css']
})

export class HomeComponent implements OnInit {
  
  constructor(
    private router: Router) { }
  ngOnInit() {}

  clearToken() {
    localStorage.removeItem('token');
    localStorage.removeItem('flag');
    this.router.navigate(['']);
  }

  
}
