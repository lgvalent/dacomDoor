import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { ModalSalaComponent } from './modal-sala.component';

describe('ModalSalaComponent', () => {
  let component: ModalSalaComponent;
  let fixture: ComponentFixture<ModalSalaComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ ModalSalaComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(ModalSalaComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
