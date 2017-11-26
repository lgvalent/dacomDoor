import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { ListAdminComponent } from './list-admin.component';

describe('ListAdminComponent', () => {
  let component: ListAdminComponent;
  let fixture: ComponentFixture<ListAdminComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ ListAdminComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(ListAdminComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
