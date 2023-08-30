import { HttpClient } from '@angular/common/http';
import { TestBed } from '@angular/core/testing';
import { Observable } from 'rxjs';

import { BackendHelperService } from './backend-helper.service';

describe('BackendHelperService', () => {
    let service: BackendHelperService;
    let httpClientSpy: jasmine.SpyObj<HttpClient>;
    const fakeUrl = 'fake/url';

    beforeEach(() => {
        httpClientSpy = jasmine.createSpyObj(
            'HttpClient',
            [
                'get',
                'post',
            ],
        );

        httpClientSpy.get.and.returnValue(new Observable<void>());
        httpClientSpy.post.and.returnValue(new Observable<void>());

        TestBed.configureTestingModule({
            providers: [
                { provide: HttpClient, useValue: httpClientSpy },
            ],
        });
        service = TestBed.inject(BackendHelperService);
    });

    it('should be created', () => {
        expect(service).toBeTruthy();
    });

    it('should call httpClient.post', () => {
        service.httpPost(fakeUrl);
        expect(httpClientSpy.post).toHaveBeenCalled();
    });

    it('should call httpClient.get', () => {
        service.httpGet(fakeUrl);
        expect(httpClientSpy.get).toHaveBeenCalled();
    });
});
