import { TestBed, inject } from '@angular/core/testing';

import { LoginDisabledService } from './login-disabled.service';

describe('LoginDisabledService', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [LoginDisabledService]
    });
  });

  it('should be created', inject([LoginDisabledService], (service: LoginDisabledService) => {
    expect(service).toBeTruthy();
  }));
});
