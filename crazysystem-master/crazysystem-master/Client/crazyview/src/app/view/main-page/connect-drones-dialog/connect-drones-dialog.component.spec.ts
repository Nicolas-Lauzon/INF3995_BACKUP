import { ComponentFixture, TestBed } from '@angular/core/testing';
import { FormsModule } from '@angular/forms';
import { MatDialogModule, MatDialogRef, MAT_DIALOG_DATA } from '@angular/material/dialog';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatInputModule } from '@angular/material/input';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { BackendService } from 'src/app/backend/backend.service';
import { ConnectDronesData, ConnectDronesDialogComponent } from './connect-drones-dialog.component';

const fakeConnectData: ConnectDronesData = { numberOfDrones: 2 };

describe('ConnectDronesDialogComponent', () => {
    let component: ConnectDronesDialogComponent;
    let fixture: ComponentFixture<ConnectDronesDialogComponent>;
    let dialogRefSpy: jasmine.SpyObj<MatDialogRef<ConnectDronesDialogComponent>>;
    let backendServiceSpy: jasmine.SpyObj<BackendService>;

    beforeEach(async () => {
        backendServiceSpy = jasmine.createSpyObj(
            'BackendService',
            [
                'connectDrones',
            ],
        );
        dialogRefSpy = jasmine.createSpyObj(
            'MatDialogRef',
            ['close'],
        );
        
        await TestBed.configureTestingModule({
            imports: [
                MatFormFieldModule,
                MatDialogModule,
                MatInputModule,
                BrowserAnimationsModule,
                FormsModule,
            ],
            declarations: [ ConnectDronesDialogComponent ],
            providers: [
                { provide: MatDialogRef, useValue: dialogRefSpy },
                { provide: MAT_DIALOG_DATA, useValue: fakeConnectData },
                { provide: BackendService, useValue: backendServiceSpy },
            ],
        }).compileComponents();
    });

    beforeEach(() => {
        fixture = TestBed.createComponent(ConnectDronesDialogComponent);
        component = fixture.componentInstance;
        fixture.detectChanges();
    });

    it('should create', () => {
        expect(component).toBeTruthy();
    });

    it('should close dialog', () => {
        component.close();
        expect(dialogRefSpy.close).toHaveBeenCalled();
    });

    it('should connect with 3 drones', () => {
        component.data.numberOfDrones = 3;
        backendServiceSpy.connectDrones.and.resolveTo('0');
        component.confirmClicked();
        expect(backendServiceSpy.connectDrones).toHaveBeenCalled();
    });

    it('should not connect with 1 drone', () => {
        component.data.numberOfDrones = 1;
        backendServiceSpy.connectDrones.and.resolveTo('0');
        component.confirmClicked();
        expect(backendServiceSpy.connectDrones).not.toHaveBeenCalled();
    });

    it('should connect with 3 drone but not close when failed response', () => {
        component.data.numberOfDrones = 3;
        backendServiceSpy.connectDrones.and.resolveTo('1');
        component.confirmClicked();
        expect(backendServiceSpy.connectDrones).toHaveBeenCalled();
    });
});
