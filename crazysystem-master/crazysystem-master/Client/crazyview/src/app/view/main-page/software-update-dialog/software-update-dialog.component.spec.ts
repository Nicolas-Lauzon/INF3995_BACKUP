import { ComponentFixture, TestBed } from '@angular/core/testing';
import { FormsModule } from '@angular/forms';
import { MatDialogModule, MatDialogRef, MAT_DIALOG_DATA } from '@angular/material/dialog';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatInputModule } from '@angular/material/input';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { BackendService } from 'src/app/backend/backend.service';
import { SoftwareUpdateData, SoftwareUpdateDialogComponent } from './software-update-dialog.component';

const fakeUpdateData: SoftwareUpdateData = { droneId: 1, file: '/test/cf2' };

describe('SoftwareUpdateDialogComponent', () => {
    let component: SoftwareUpdateDialogComponent;
    let fixture: ComponentFixture<SoftwareUpdateDialogComponent>;
    let dialogRefSpy: jasmine.SpyObj<MatDialogRef<SoftwareUpdateDialogComponent>>;
    let backendServiceSpy: jasmine.SpyObj<BackendService>;

    beforeEach(async () => {
        backendServiceSpy = jasmine.createSpyObj(
            'BackendService',
            [
                'softwareUpdate',
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
            declarations: [ SoftwareUpdateDialogComponent ],
            providers: [
                { provide: MatDialogRef, useValue: dialogRefSpy },
                { provide: MAT_DIALOG_DATA, useValue: fakeUpdateData },
                { provide: BackendService, useValue: backendServiceSpy },
            ],
        }).compileComponents();
    });

    beforeEach(() => {
        fixture = TestBed.createComponent(SoftwareUpdateDialogComponent);
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

    it('should call software update', () => {
        component.data.droneId = 1;
        component.data.file = '/test/cf2';
        backendServiceSpy.softwareUpdate.and.resolveTo('0')
        component.confirmClicked();
        expect(backendServiceSpy.softwareUpdate).toHaveBeenCalledWith(1,'/home/test/cf2.bin');
    });

    it('should call software update but fail response', () => {
        component.data.droneId = 1;
        component.data.file = '/test/cf2';
        backendServiceSpy.softwareUpdate.and.resolveTo('1')
        component.confirmClicked();
        expect(backendServiceSpy.softwareUpdate).toHaveBeenCalledWith(1,'/home/test/cf2.bin');
    });
});
