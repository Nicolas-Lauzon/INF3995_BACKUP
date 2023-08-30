import { TestBed } from '@angular/core/testing';
import { BackendAPI } from './backend-api';
import { BackendHelperService } from './backend-helper.service';

import { BackendService } from './backend.service';

describe('BackendService', () => {
    let service: BackendService;
    let backendHelperServiceSpy: jasmine.SpyObj<BackendHelperService>;

    beforeEach(() => {
        backendHelperServiceSpy = jasmine.createSpyObj(
            'BackendHelperService',
            [
                'httpGet',
                'httpPost',
            ],
        );

        TestBed.configureTestingModule({
            providers: [
                { provide: BackendHelperService, useValue: backendHelperServiceSpy },
            ],
        });
        service = TestBed.inject(BackendService);
    });

    it('should be created', () => {
        expect(service).toBeTruthy();
    });

    it('should call backendHelper.httpPost with backendAPI.connect url', () => {
        var randomNumber = Math.floor(Math.random() * (99 - 2 +1) + 2); 
        service.connectDrones(randomNumber);
        expect(backendHelperServiceSpy.httpPost).toHaveBeenCalledWith(BackendAPI.connectDrones(1, randomNumber));
    });

    it('should call backendHelper.httpPost with BackendAPI.takeoff url ', () => {
        service.takeOffDrone();
        expect(backendHelperServiceSpy.httpPost).toHaveBeenCalledWith(BackendAPI.takeoff());
    });

    it('should call backendHelper.httpPost with BackendAPI.land url ', () => {
        service.landDrone();
        expect(backendHelperServiceSpy.httpPost).toHaveBeenCalledWith(BackendAPI.land());
    });

    it('should call backendHelper.httpPost with BackendAPI.softwareUpdate url ', () => {
        const droneID = 1;
        const filePath = "/home/test/cf2.bin"
        service.softwareUpdate(droneID, filePath);
        expect(backendHelperServiceSpy.httpPost).toHaveBeenCalledWith(BackendAPI.softwareUpdate(droneID, filePath));
    });

    it('should call backendHelper.httpPost with BackendAPI.returnToBase url ', () => {
        service.returnToBase();
        expect(backendHelperServiceSpy.httpPost).toHaveBeenCalledWith(BackendAPI.returnToBase());
    });

    it('should call backendHelper.httpGet with BackendAPI.getPointsAtStart url ', () => {
        service.getMapPointsAtStart();
        expect(backendHelperServiceSpy.httpGet).toHaveBeenCalledWith(BackendAPI.getPointsAtStart());
    });
});
