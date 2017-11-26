import { ListAdminModule } from './list-admin.module';

describe('ListAdminModule', () => {
  let listAdminModule: ListAdminModule;

  beforeEach(() => {
    listAdminModule = new ListAdminModule();
  });

  it('should create an instance', () => {
    expect(listAdminModule).toBeTruthy();
  });
});
