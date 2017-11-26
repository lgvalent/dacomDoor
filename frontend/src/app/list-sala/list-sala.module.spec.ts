import { ListSalaModule } from './list-sala.module';

describe('ListSalaModule', () => {
  let listSalaModule: ListSalaModule;

  beforeEach(() => {
    listSalaModule = new ListSalaModule();
  });

  it('should create an instance', () => {
    expect(listSalaModule).toBeTruthy();
  });
});
