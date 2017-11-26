import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { ModalHorarioComponent } from './modal-horario.component';

describe('ModalHorarioComponent', () => {
  let component: ModalHorarioComponent;
  let fixture: ComponentFixture<ModalHorarioComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ ModalHorarioComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(ModalHorarioComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
