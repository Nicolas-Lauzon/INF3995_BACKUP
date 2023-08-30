export const BackendAPI = {
    connectDrones(connectID: number, param: number): string {
        return `connect?${connectID}=${param}`;
    },

    takeoff(): string {
        return 'takeoff';
    },

    land(): string {
        return 'land';
    },

    softwareUpdate(droneID:number, file:string): string {
        return `softwareUpdate?${droneID}=${file}`;
    },

    returnToBase(): string {
        return 'returnToBase';
    },

    getPointsAtStart(): string{
        return 'refreshPoints'
    },
};