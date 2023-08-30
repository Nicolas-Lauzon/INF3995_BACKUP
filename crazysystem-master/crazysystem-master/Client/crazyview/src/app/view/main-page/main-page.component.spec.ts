import { ComponentFixture, TestBed } from '@angular/core/testing';
import { MatButtonModule } from '@angular/material/button';
import { MatCardModule } from '@angular/material/card';
import { MatDialog, MatDialogModule, MatDialogRef } from '@angular/material/dialog';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatTableModule } from '@angular/material/table';
import { MatToolbarModule } from '@angular/material/toolbar';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { Observable } from 'rxjs';
import { Socket } from 'socket.io-client';
import { BackendService } from 'src/app/backend/backend.service';
import { DroneState } from 'src/app/backend/interfaces/client';
import { SocketHelperService } from 'src/app/backend/socket-helper.service';
import { DroneInfoComponent } from '../drone-info/drone-info.component';
import { fakeDrone1, fakeDrone2 } from '../drone-info/drone-info.component.spec';
import { MapComponent } from '../map/map.component';
import { ConnectDronesDialogComponent } from './connect-drones-dialog/connect-drones-dialog.component';
import { HelpDialogComponent } from './help-dialog/help-dialog.component';

import { MainPageComponent } from './main-page.component';
import { SoftwareUpdateDialogComponent } from './software-update-dialog/software-update-dialog.component';

describe('MainPageComponent', () => {
    let component: MainPageComponent;
    let fixture: ComponentFixture<MainPageComponent>;
    let backendServiceSpy: jasmine.SpyObj<BackendService>;
    let socketHelperSpy: jasmine.SpyObj<SocketHelperService>;
    let socketSpy: jasmine.SpyObj<Socket>;
    let dialogSpy: jasmine.SpyObj<MatDialog>;
    let dialogRefUpdateSpy: jasmine.SpyObj<MatDialogRef<SoftwareUpdateDialogComponent>>;
    let dialogRefConnectSpy: jasmine.SpyObj<MatDialogRef<ConnectDronesDialogComponent>>;

    beforeEach(async () => {
        backendServiceSpy = jasmine.createSpyObj(
            'BackendService',
            [
                'takeOffDrone',
                'landDrone',
                'softwareUpdate',
                'returnToBase',
                'connectDrones',
            ],
        );
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
        dialogSpy = jasmine.createSpyObj(
            'MatDialog',
            [ 'open' ],
        );
        dialogRefUpdateSpy = jasmine.createSpyObj(
            'MatDialogRef',
            [ 'afterClosed' ],
        );
        dialogRefConnectSpy = jasmine.createSpyObj(
            'MatDialogRef',
            [ 'afterClosed' ],
        );
        dialogRefUpdateSpy.afterClosed.and.returnValue(new Observable());
        dialogRefConnectSpy.afterClosed.and.returnValue(new Observable());

        await TestBed.configureTestingModule({
            imports: [
                MatToolbarModule,
                MatButtonModule,
                MatTableModule,
                MatCardModule,
                MatDialogModule,
                MatIconModule,
                MatFormFieldModule,
                BrowserAnimationsModule,
            ],
            declarations: [
                MainPageComponent,
                DroneInfoComponent,
                MapComponent,
                ConnectDronesDialogComponent,
                HelpDialogComponent,
                SoftwareUpdateDialogComponent,
            ],
            providers: [
                { provide: BackendService, useValue: backendServiceSpy },
                { provide: SocketHelperService, useValue: socketHelperSpy },
                { provide: MatDialog, useValue: dialogSpy },
            ],
        }).compileComponents();

        socketHelperSpy.socket = socketSpy;
    });

    beforeEach(() => {
        fixture = TestBed.createComponent(MainPageComponent);
        component = fixture.componentInstance;
        fixture.detectChanges();
    });

    it('should create', () => {
        expect(component).toBeTruthy();
    });

    it('should make drone take off', () => {
        component.takeOffDrone();
        expect(backendServiceSpy.takeOffDrone).toHaveBeenCalled();
    });

    it('should make drone land', () => {
        component.landDrone();
        expect(backendServiceSpy.landDrone).toHaveBeenCalled();
    });

    it('should make drone return to base', () => {
        component.returnToBase();
        expect(backendServiceSpy.returnToBase).toHaveBeenCalled();
    });

    it('should open the connect drones dialog', () => {
        dialogSpy.open.and.returnValue(dialogRefConnectSpy);
        component.openConnectDrones();
        expect(component['dialog'].open).toHaveBeenCalled();
    });

    it('should open the software update dialog', () => {
        dialogSpy.open.and.returnValue(dialogRefUpdateSpy);
        component.softwareDialog();
        expect(component['dialog'].open).toHaveBeenCalled();
    });

    it('should open the help dialog', () => {
        component.openHelp();
        expect(component['dialog'].open).toHaveBeenCalled();
    });

    it('should add log at beginning of logs, and pop last value', () => {
        component.logs = [
            'LOG 1', 'LOG 2', 'LOG 3', 'LOG 4', 'LOG 5', 'LOG 6', 'LOG 7', 'LOG 8', 'LOG 9', 'LOG 10', 
            'LOG 11', 'LOG 12', 'LOG 13', 'LOG 14', 'LOG 15', 'LOG 16', 'LOG 17', 'LOG 18', 'LOG 19', 
        ];
        const fakeLog = 'new LOG';
        component['onLogReception'](fakeLog);
        expect(component.logs[0]).toEqual(fakeLog);
        expect(component.logs.length).toEqual(19);
    });

    it('should add log at beginning of logs, and not pop last value', () => {
        component.logs = ['LOG 1', 'LOG 2', 'LOG 3', 'LOG 4', 'LOG 5', 'LOG 6', 'LOG 7', 'LOG 8', 'LOG 9', 'LOG 10'];
        const fakeLog = 'new LOG';
        component['onLogReception'](fakeLog);
        expect(component.logs[0]).toEqual(fakeLog);
        expect(component.logs.length).toEqual(11);
    });

    it('should update button enabling with all drones standby', () => {
        component.takeOffEnable = false;
        component.landReturnEnable = false;
        component.updateEnable = false;
        const fakeDrones = [fakeDrone1, fakeDrone1];
        component.updateButtonEnabling(fakeDrones);
        expect(component.takeOffEnable).toBeTruthy();
        expect(component.landReturnEnable).toBeFalsy();
        expect(component.updateEnable).toBeTruthy();
    });

    it('should update button enabling with all drones inMission', () => {
        component.takeOffEnable = false;
        component.landReturnEnable = false;
        component.updateEnable = false;
        let fakeDrone = fakeDrone2;
        fakeDrone.State = DroneState.InMission;
        const fakeDrones = [fakeDrone, fakeDrone];
        component.updateButtonEnabling(fakeDrones);
        expect(component.takeOffEnable).toBeFalsy();
        expect(component.landReturnEnable).toBeTruthy();
        expect(component.updateEnable).toBeFalsy();
    });
});
