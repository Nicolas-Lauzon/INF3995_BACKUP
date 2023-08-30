import { TestBed } from '@angular/core/testing';
import { DroneInfo, DroneState } from './interfaces/client';
import { ServDroneInfo } from './interfaces/server';

import { SocketHelperService } from './socket-helper.service';


export const fakeServDrone: ServDroneInfo =
    {
        id: 1,
        batteryLevel: 69,
        speed: 69,
        state: 0,
    };

export const fakeDrone: DroneInfo =
    {
        Id: 1,
        BatteryLevel: 69,
        Speed: 69,
        State: DroneState.Standby,
    };


describe('SocketHelperService', () => {
    let service: SocketHelperService;

    beforeEach(() => {
        TestBed.configureTestingModule({});
        service = TestBed.inject(SocketHelperService);
    });

    it('should be created', () => {
        expect(service).toBeTruthy();
    });

    it('should convert drones info', () => {
        const result = service.convertDronesInfos(fakeServDrone);
        expect(result).toEqual(fakeDrone);
    });
});
