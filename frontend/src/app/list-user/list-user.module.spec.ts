import { ListUserModule } from './list-user.module';

describe('ListUserModule', () => {
  let listUserModule: ListUserModule;

  beforeEach(() => {
    listUserModule = new ListUserModule();
  });

  it('should create an instance', () => {
    expect(listUserModule).toBeTruthy();
  });
});
