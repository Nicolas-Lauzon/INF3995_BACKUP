import { ComponentFixture, TestBed } from '@angular/core/testing';
import { Socket } from 'socket.io-client';
import { BackendService } from 'src/app/backend/backend.service';
import { ServPoint } from 'src/app/backend/interfaces/server';
import { SocketHelperService } from 'src/app/backend/socket-helper.service';

import { MapComponent } from './map.component';

const fakePoint: ServPoint = {
    coordX: 69,
    coordY: 42,
};

describe('MapComponent', () => {
    let component: MapComponent;
    let fixture: ComponentFixture<MapComponent>;
    let socketHelperSpy: jasmine.SpyObj<SocketHelperService>;
    let socketSpy: jasmine.SpyObj<Socket>;
    let contextSpy: jasmine.SpyObj<CanvasRenderingContext2D>;
    let backendServiceSpy: jasmine.SpyObj<BackendService>;

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
        contextSpy = jasmine.createSpyObj(
            'CanvasRenderingContext2D',
            [
                'beginPath',
                'arc',
                'fill',
                'clearRect',
            ],
        );
        backendServiceSpy = jasmine.createSpyObj(
            'BackendService',
            [ 'getMapPointsAtStart' ],
        );
        backendServiceSpy.getMapPointsAtStart.and.resolveTo([]);

        await TestBed.configureTestingModule({
            declarations: [MapComponent],
            providers: [
                { provide: SocketHelperService, useValue: socketHelperSpy },
                { provide: BackendService, useValue: backendServiceSpy },
            ],
        }).compileComponents();

        socketHelperSpy.socket = socketSpy;
    });

    beforeEach(() => {
        fixture = TestBed.createComponent(MapComponent);
        component = fixture.componentInstance;
        fixture.detectChanges();
        component['context'] = contextSpy;
    });

    it('should create', () => {
        expect(component).toBeTruthy();
    });

    it('should initialise points', () => {
        expect(backendServiceSpy.getMapPointsAtStart).toHaveBeenCalled();
    });

    it('should draw a new point and update last update', () => {
        component['onNewPoints']([fakePoint]);
        expect(contextSpy.beginPath).toHaveBeenCalledTimes(1);
        expect(contextSpy.arc).toHaveBeenCalledOnceWith(
            fakePoint.coordX,
            fakePoint.coordY,
            1,
            0,
            2 * Math.PI,
            true
        );
        expect(contextSpy.beginPath).toHaveBeenCalledTimes(1);
        expect(contextSpy.fillStyle).toBe("black");
        expect(component.lastUpdate).not.toBeNull();
    });

    it('should draw a new drone if never drawn', () => {
        component['moveDrone'](1, fakePoint);
        expect(contextSpy.beginPath).toHaveBeenCalledTimes(1);
        expect(contextSpy.arc).toHaveBeenCalledOnceWith(
            fakePoint.coordX,
            fakePoint.coordY,
            2,
            0,
            2 * Math.PI,
            true
        );
        expect(contextSpy.beginPath).toHaveBeenCalledTimes(1);
        expect(contextSpy.fillStyle).toBe("red");
        expect(contextSpy.clearRect).not.toHaveBeenCalled();
        expect(component.dronePositions.get(1)).toBe(fakePoint);
    });

    it('should move drone if already drawn', () => {
        component.dronePositions.set(1, { coordX: 50, coordY: 50 })
        component['moveDrone'](1, fakePoint);
        expect(contextSpy.beginPath).toHaveBeenCalledTimes(1);
        expect(contextSpy.arc).toHaveBeenCalledOnceWith(
            fakePoint.coordX,
            fakePoint.coordY,
            2,
            0,
            2 * Math.PI,
            true
        );
        expect(contextSpy.beginPath).toHaveBeenCalledTimes(1);
        expect(contextSpy.fillStyle).toBe("red");
        expect(contextSpy.clearRect).toHaveBeenCalledWith(47, 47, 6, 6);
        expect(component.dronePositions.get(1)).toBe(fakePoint);
    });
});
