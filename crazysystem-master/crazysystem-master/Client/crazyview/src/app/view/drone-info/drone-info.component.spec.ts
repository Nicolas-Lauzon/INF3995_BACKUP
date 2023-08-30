import { ComponentFixture, TestBed } from '@angular/core/testing';
import { MatCardModule } from '@angular/material/card';
import { MatTableModule } from '@angular/material/table';
import { Socket } from 'socket.io-client';
import { DroneInfo, DroneState } from 'src/app/backend/interfaces/client';
import { SocketHelperService } from 'src/app/backend/socket-helper.service';
import { ServDroneInfo } from 'src/app/backend/interfaces/server';

import { DroneInfoComponent } from './drone-info.component';

export const fakeDrone1: DroneInfo = {
    Id: 1,
    BatteryLevel: 69,
    Speed: 69,
    State: DroneState.Standby,
};

export const fakeDrone2: DroneInfo = {
    Id: 2,
    BatteryLevel: 42,
    Speed: 42,
    State: DroneState.Crashed,
};

export const fakeServDrone1: ServDroneInfo = {
    id: 1,
    batteryLevel: 69,
    speed: 69,
    state: 0,
};

export const fakeServDrone2: ServDroneInfo = {
    id: 2,
    batteryLevel: 42,
    speed: 42,
    state: 2,
};

export const fakeDrone: DroneInfo[] = [fakeDrone1];
export const fakeDrones2: DroneInfo[] = [fakeDrone1, fakeDrone2];

describe('DroneInfoComponent', () => {
    let component: DroneInfoComponent;
    let fixture: ComponentFixture<DroneInfoComponent>;
    let socketHelperSpy: jasmine.SpyObj<SocketHelperService>;
    let socketSpy: jasmine.SpyObj<Socket>;

    beforeEach(async () => {
        socketHelperSpy = jasmine.createSpyObj(
            'SocketHelperService',
            [
                'convertDronesInfos',
            ],
        );
        socketSpy = jasmine.createSpyObj(
            'Socket',
            [
                'on',
            ],
        );

        await TestBed.configureTestingModule({
            imports: [
                MatTableModule,
                MatCardModule,
            ],
            declarations: [ DroneInfoComponent ],
            providers: [
                { provide: SocketHelperService, useValue: socketHelperSpy },
            ],
        })
        .compileComponents();
        socketHelperSpy.socket = socketSpy;
    });

    beforeEach(() => {
        fixture = TestBed.createComponent(DroneInfoComponent);
        component = fixture.componentInstance;
        fixture.detectChanges();
    });

    it('should create', () => {
        expect(component).toBeTruthy();
    });

    it('should convert drones and update drones', () => {
        socketHelperSpy.convertDronesInfos.and.returnValue(fakeDrone1);
        // tslint:disable-next-line: no-string-literal
        component['onInfo'](fakeServDrone1);
        expect(socketHelperSpy.convertDronesInfos).toHaveBeenCalled();
        expect(component.drones).toEqual(fakeDrone);
        expect(component.numberOfDrones).toEqual(fakeDrone.length);
    });

    it('should convert drones and update drones', () => {
        socketHelperSpy.convertDronesInfos.and.returnValue(fakeDrone2);
        component.drones = fakeDrone;
        // tslint:disable-next-line: no-string-literal
        component['onInfo'](fakeServDrone2);
        expect(socketHelperSpy.convertDronesInfos).toHaveBeenCalled();
        expect(component.drones).toEqual(fakeDrones2);
        expect(component.numberOfDrones).toEqual(fakeDrones2.length);
    });

});
