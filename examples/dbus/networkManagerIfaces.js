// AUTOMATICALLY GENERATED INTERFACE FILE
// Do not edit by hand.


// NM_802_11_MODE in generic-types
//   UNKNOWN: Mode is unknown.
//   ADHOC: Uncoordinated network without central infrastructure.
//   INFRA: Coordinated network with one or more central controllers.

const NM_802_11_MODE_UNKNOWN = 0;
const NM_802_11_MODE_ADHOC = 1;
const NM_802_11_MODE_INFRA = 2;


var NetworkManagerAccessPoint = {
    name: 'org.freedesktop.NetworkManager.AccessPoint',
    methods: [],
    signals: [

        // PropertiesChanged:
        // In Args:
        //   properties: (String_Variant_Map) A dictionary mapping property
        // names to variant boxed values

        { name: 'PropertiesChanged', inSignature: 'a{sv}', outSignature: '' }
    ],
    properties: [

        // Flags: (NM_802_11_AP_FLAGS) Flags describing the capabilities of
        // the access point.

        { name: 'Flags', signature: 'u', access: 'read' },

        // WpaFlags: (NM_802_11_AP_SEC) Flags describing the access point's
        // capabilities according to WPA (Wifi Protected Access).

        { name: 'WpaFlags', signature: 'u', access: 'read' },

        // RsnFlags: (NM_802_11_AP_SEC) Flags describing the access point's
        // capabilities according to the RSN (Robust Secure Network) protocol.

        { name: 'RsnFlags', signature: 'u', access: 'read' },

        // Ssid: The Service Set Identifier identifying the access point.

        { name: 'Ssid', signature: 'ay', access: 'read' },

        // Frequency: The radio channel frequency in use by the access point,
        // in MHz.

        { name: 'Frequency', signature: 'u', access: 'read' },

        // HwAddress: The hardware address (BSSID) of the access point.

        { name: 'HwAddress', signature: 's', access: 'read' },

        // Mode: (NM_802_11_MODE) Describes the operating mode of the access
        // point.

        { name: 'Mode', signature: 'u', access: 'read' },

        // MaxBitrate: The maximum bitrate this access point is capable of, in
        // kilobits/second (Kb/s).

        { name: 'MaxBitrate', signature: 'u', access: 'read' },

        // Strength: The current signal quality of the access point, in
        // percent.

        { name: 'Strength', signature: 'y', access: 'read' }
    ]
};

// NM_802_11_AP_FLAGS in org.freedesktop.NetworkManager.AccessPoint
//   NONE: Null capability - says nothing about the access point.
//   PRIVACY: Access point supports privacy measures.

const NM_802_11_AP_FLAGS_NONE = 0x0;
const NM_802_11_AP_FLAGS_PRIVACY = 0x1;


// NM_802_11_AP_SEC in org.freedesktop.NetworkManager.AccessPoint
//   NONE: Null flag.
//   PAIR_WEP40: Access point supports pairwise 40-bit WEP encryption.
//   PAIR_WEP104: Access point supports pairwise 104-bit WEP
// encryption.
//   PAIR_TKIP: Access point supports pairwise TKIP encryption.
//   PAIR_CCMP: Access point supports pairwise CCMP encryption.
//   GROUP_WEP40: Access point supports a group 40-bit WEP cipher.
//   GROUP_WEP104: Access point supports a group 104-bit WEP cipher.
//   GROUP_TKIP: Access point supports a group TKIP cipher.
//   GROUP_CCMP: Access point supports a group CCMP cipher.
//   KEY_MGMT_PSK: Access point supports PSK key management.
//   KEY_MGMT_802_1X: Access point supports 802.1x key management.

const NM_802_11_AP_SEC_NONE = 0x0;
const NM_802_11_AP_SEC_PAIR_WEP40 = 0x1;
const NM_802_11_AP_SEC_PAIR_WEP104 = 0x2;
const NM_802_11_AP_SEC_PAIR_TKIP = 0x4;
const NM_802_11_AP_SEC_PAIR_CCMP = 0x8;
const NM_802_11_AP_SEC_GROUP_WEP40 = 0x10;
const NM_802_11_AP_SEC_GROUP_WEP104 = 0x20;
const NM_802_11_AP_SEC_GROUP_TKIP = 0x40;
const NM_802_11_AP_SEC_GROUP_CCMP = 0x80;
const NM_802_11_AP_SEC_KEY_MGMT_PSK = 0x100;
const NM_802_11_AP_SEC_KEY_MGMT_802_1X = 0x200;


var NetworkManagerConnectionActive = {
    name: 'org.freedesktop.NetworkManager.Connection.Active',
    methods: [],
    signals: [

        // PropertiesChanged:
        // In Args:
        //   properties: (String_Variant_Map) A dictionary mapping property
        // names to variant boxed values

        { name: 'PropertiesChanged', inSignature: 'a{sv}', outSignature: '' }
    ],
    properties: [

        // ServiceName: The D-Bus service name providing this connection.

        { name: 'ServiceName', signature: 's', access: 'read' },

        // Connection: The path of the connection.

        { name: 'Connection', signature: 'o', access: 'read' },

        // SpecificObject: A specific object associated with the active
        // connection.

        { name: 'SpecificObject', signature: 'o', access: 'read' },

        // Devices: Array of object paths representing devices which are part
        // of this active connection.

        { name: 'Devices', signature: 'ao', access: 'read' },

        // State: (NM_ACTIVE_CONNECTION_STATE) The state of this active
        // connection.

        { name: 'State', signature: 'u', access: 'read' },

        // Default: Whether this active connection is the default connection,
        // i.e. whether it currently owns the default route.

        { name: 'Default', signature: 'b', access: 'read' }
    ]
};

// NM_ACTIVE_CONNECTION_STATE in org.freedesktop.NetworkManager.Connection.Active
//   UNKNOWN: The active connection is in an unknown state.
//   ACTIVATING: The connection is activating.
//   ACTIVATED: The connection is activated.

const NM_ACTIVE_CONNECTION_STATE_UNKNOWN = 0;
const NM_ACTIVE_CONNECTION_STATE_ACTIVATING = 1;
const NM_ACTIVE_CONNECTION_STATE_ACTIVATED = 2;


var NetworkManagerDeviceCdma = {
    name: 'org.freedesktop.NetworkManager.Device.Cdma',
    methods: [],
    signals: [

        // PropertiesChanged:
        // In Args:
        //   properties: (String_Variant_Map) A dictionary mapping property
        // names to variant boxed values

        { name: 'PropertiesChanged', inSignature: 'a{sv}', outSignature: '' }
    ],
    properties: []
};

var NetworkManagerDeviceWired = {
    name: 'org.freedesktop.NetworkManager.Device.Wired',
    methods: [],
    signals: [

        // PropertiesChanged:
        // In Args:
        //   properties: (String_Variant_Map) A dictionary mapping property
        // names to variant boxed values

        { name: 'PropertiesChanged', inSignature: 'a{sv}', outSignature: '' }
    ],
    properties: [

        // HwAddress: Hardware address of the device.

        { name: 'HwAddress', signature: 's', access: 'read' },

        // Speed: Design speed of the device.

        { name: 'Speed', signature: 'u', access: 'read' },

        // Carrier: Indicates whether the physical carrier is found (e.g.
        // whether a cable is plugged in or not).

        { name: 'Carrier', signature: 'b', access: 'read' }
    ]
};

var NetworkManagerDeviceGsm = {
    name: 'org.freedesktop.NetworkManager.Device.Gsm',
    methods: [],
    signals: [

        // PropertiesChanged:
        // In Args:
        //   properties: (String_Variant_Map) A dictionary mapping property
        // names to variant boxed values

        { name: 'PropertiesChanged', inSignature: 'a{sv}', outSignature: '' }
    ],
    properties: []
};

var NetworkManagerDeviceSerial = {
    name: 'org.freedesktop.NetworkManager.Device.Serial',
    methods: [],
    signals: [

        // PppStats:
        // In Args:
        //   in_bytes: Bytes received
        //   out_bytes: Bytes sent

        { name: 'PppStats', inSignature: 'uu', outSignature: '' }
    ],
    properties: []
};

var NetworkManagerDeviceWireless = {
    name: 'org.freedesktop.NetworkManager.Device.Wireless',
    methods: [

        // GetAccessPoints:
        // Out Args:
        //   access_points: List of access point object paths

        { name: 'GetAccessPoints', inSignature: '', outSignature: 'ao' },

        // ScanNow

        { name: 'ScanNow', inSignature: '', outSignature: '' }
    ],
    signals: [

        // PropertiesChanged:
        // In Args:
        //   properties: (String_Variant_Map) A dictionary containing the
        // FIXME: check changed parameters.

        { name: 'PropertiesChanged', inSignature: 'a{sv}', outSignature: '' },

        // AccessPointAdded:
        // In Args:
        //   access_point: The object path of the newly found access point.

        { name: 'AccessPointAdded', inSignature: 'o', outSignature: '' },

        // AccessPointRemoved:
        // In Args:
        //   access_point: The object path of the access point that has
        // disappeared.

        { name: 'AccessPointRemoved', inSignature: 'o', outSignature: '' }
    ],
    properties: [

        // HwAddress: The hardware address of the device.

        { name: 'HwAddress', signature: 's', access: 'read' },

        // Mode: (NM_802_11_MODE) The operating mode of the wireless device.

        { name: 'Mode', signature: 'u', access: 'read' },

        // Bitrate: The bit rate currently used by the wireless device, in
        // kilobits/second (Kb/s).

        { name: 'Bitrate', signature: 'u', access: 'read' },

        // ActiveAccessPoint: Object path of the access point currently used
        // by the wireless device.

        { name: 'ActiveAccessPoint', signature: 'o', access: 'read' },

        // WirelessCapabilities: (NM_802_11_DEVICE_CAP) The capabilities of
        // the wireless device.

        { name: 'WirelessCapabilities', signature: 'u', access: 'read' }
    ]
};

// NM_802_11_DEVICE_CAP in org.freedesktop.NetworkManager.Device.Wireless
//   NONE: Null capability - syntactic sugar for no capabilities
// supported. Do not AND this with other capabilities!
//   CIPHER_WEP40: The device supports the 40-bit WEP cipher.
//   CIPHER_WEP104: The device supports the 104-bit WEP cipher.
//   CIPHER_TKIP: The device supports the TKIP cipher.
//   CIPHER_CCMP: The device supports the CCMP cipher.
//   WPA: The device supports the WPA encryption/authentication
// protocol.
//   RSN: The device supports the RSN encryption/authentication
// protocol.

const NM_802_11_DEVICE_CAP_NONE = 0x0;
const NM_802_11_DEVICE_CAP_CIPHER_WEP40 = 0x1;
const NM_802_11_DEVICE_CAP_CIPHER_WEP104 = 0x2;
const NM_802_11_DEVICE_CAP_CIPHER_TKIP = 0x4;
const NM_802_11_DEVICE_CAP_CIPHER_CCMP = 0x8;
const NM_802_11_DEVICE_CAP_WPA = 0x10;
const NM_802_11_DEVICE_CAP_RSN = 0x20;


var NetworkManagerDevice = {
    name: 'org.freedesktop.NetworkManager.Device',
    methods: [],
    signals: [

        // StateChanged:
        // In Args:
        //   new_state: (NM_DEVICE_STATE) The new state of the device.
        //   old_state: (NM_DEVICE_STATE) The previous state of the device.
        //   reason: (NM_DEVICE_STATE_REASON) A reason for the state
        // transition.

        { name: 'StateChanged', inSignature: 'uuu', outSignature: '' }
    ],
    properties: [

        // Udi: HAL UDI for the device.

        { name: 'Udi', signature: 's', access: 'read' },

        // Interface: The network interface offered by the device.

        { name: 'Interface', signature: 's', access: 'read' },

        // Driver: The driver handling the device.

        { name: 'Driver', signature: 's', access: 'read' },

        // Capabilities: (NM_DEVICE_CAP) Flags describing the capabilities of
        // the device.

        { name: 'Capabilities', signature: 'u', access: 'read' },

        // Ip4Address: The IPv4 address bound to the device. FIXME: what about
        // devices with >1 IP address?

        { name: 'Ip4Address', signature: 'i', access: 'read' },

        // State: (NM_DEVICE_STATE) The current state of the device.

        { name: 'State', signature: 'u', access: 'read' },

        // Ip4Config: Object path of the Ip4Config object describing the
        // configuration of the device. Only valid when the device is in the
        // NM_DEVICE_STATE_ACTIVATED state.

        { name: 'Ip4Config', signature: 'o', access: 'read' },

        // Dhcp4Config: Object path of the Dhcp4Config object describing the
        // DHCP options returned by the DHCP server. Only valid when the
        // device is in the NM_DEVICE_STATE_ACTIVATED state.

        { name: 'Dhcp4Config', signature: 'o', access: 'read' },

        // Managed: Whether or not this device is managed by NetworkManager.

        { name: 'Managed', signature: 'b', access: 'read' },

        // DeviceType:

        { name: 'DeviceType', signature: 'u', access: 'read' }
    ]
};

// NM_DEVICE_STATE in org.freedesktop.NetworkManager.Device
//   UNKNOWN: The device is in an unknown state.
//   UNMANAGED: The device is not managed by NetworkManager.
//   UNAVAILABLE: The device cannot be used (carrier off, rfkill,
// etc).
//   DISCONNECTED: The device is not connected.
//   PREPARE: The device is preparing to connect.
//   CONFIG: The device is being configured.
//   NEED_AUTH: The device is awaiting secrets necessary to continue
// connection.
//   IP_CONFIG: The IP settings of the device are being requested and
// configured.
//   ACTIVATED: The device is active.
//   FAILED: The device is in a failure state following an attempt to
// activate it.

const NM_DEVICE_STATE_UNKNOWN = 0;
const NM_DEVICE_STATE_UNMANAGED = 1;
const NM_DEVICE_STATE_UNAVAILABLE = 2;
const NM_DEVICE_STATE_DISCONNECTED = 3;
const NM_DEVICE_STATE_PREPARE = 4;
const NM_DEVICE_STATE_CONFIG = 5;
const NM_DEVICE_STATE_NEED_AUTH = 6;
const NM_DEVICE_STATE_IP_CONFIG = 7;
const NM_DEVICE_STATE_ACTIVATED = 8;
const NM_DEVICE_STATE_FAILED = 9;


// NM_DEVICE_STATE_REASON in org.freedesktop.NetworkManager.Device
//   UNKNOWN: The reason for the device state change is unknown.
//   NONE: The state change is normal.
//   NOW_MANAGED: The device is now managed.
//   NOW_UNMANAGED: The device is no longer managed.
//   CONFIG_FAILED: The device could not be readied for configuration.
//   CONFIG_UNAVAILABLE: IP configuration could not be reserved (no
// available address, timeout, etc).
//   CONFIG_EXPIRED: The IP configuration is no longer valid.
//   NO_SECRETS: Secrets were required, but not provided.
//   SUPPLICANT_DISCONNECT: The 802.1X supplicant disconnected from
// the access point or authentication server.
//   SUPPLICANT_CONFIG_FAILED: Configuration of the 802.1X supplicant
// failed.
//   SUPPLICANT_FAILED: The 802.1X supplicant quit or failed
// unexpectedly.
//   SUPPLICANT_TIMEOUT: The 802.1X supplicant took too long to
// authenticate.
//   PPP_START_FAILED: The PPP service failed to start within the
// allowed time.
//   PPP_DISCONNECT: The PPP service disconnected unexpectedly.
//   PPP_FAILED: The PPP service quit or failed unexpectedly.
//   DHCP_START_FAILED: The DHCP service failed to start within the
// allowed time.
//   DHCP_ERROR: The DHCP service reported an unexpected error.
//   DHCP_FAILED: The DHCP service quit or failed unexpectedly.
//   SHARED_START_FAILED: The shared connection service failed to
// start.
//   SHARED_FAILED: The shared connection service quit or failed
// unexpectedly.
//   AUTOIP_START_FAILED: The AutoIP service failed to start.
//   AUTOIP_ERROR: The AutoIP service reported an unexpected error.
//   AUTOIP_FAILED: The AutoIP service quit or failed unexpectedly.
//   MODEM_BUSY: Dialing failed because the line was busy.
//   MODEM_NO_DIAL_TONE: Dialing failed because there was no dial
// tone.
//   MODEM_NO_CARRIER: Dialing failed because there was carrier.
//   MODEM_DIAL_TIMEOUT: Dialing timed out.
//   MODEM_DIAL_FAILED: Dialing failed.
//   MODEM_INIT_FAILED: Modem initialization failed.
//   GSM_APN_FAILED: Failed to select the specified GSM APN.
//   GSM_REGISTRATION_NOT_SEARCHING: Not searching for networks.
//   GSM_REGISTRATION_DENIED: Network registration was denied.
//   GSM_REGISTRATION_TIMEOUT: Network registration timed out.
//   GSM_REGISTRATION_FAILED: Failed to register with the requested
// GSM network.
//   GSM_PIN_CHECK_FAILED: PIN check failed.
//   FIRMWARE_MISSING: Necessary firmware for the device may be
// missing.
//   REMOVED: The device was removed.
//   SLEEPING: NetworkManager went to sleep.
//   CONNECTION_REMOVED: The device's active connection was removed or
// disappeared.
//   USER_REQUESTED: A user or client requested the disconnection.
//   CARRIER: The device's carrier/link changed.

const NM_DEVICE_STATE_REASON_UNKNOWN = 0;
const NM_DEVICE_STATE_REASON_NONE = 1;
const NM_DEVICE_STATE_REASON_NOW_MANAGED = 2;
const NM_DEVICE_STATE_REASON_NOW_UNMANAGED = 3;
const NM_DEVICE_STATE_REASON_CONFIG_FAILED = 4;
const NM_DEVICE_STATE_REASON_CONFIG_UNAVAILABLE = 5;
const NM_DEVICE_STATE_REASON_CONFIG_EXPIRED = 6;
const NM_DEVICE_STATE_REASON_NO_SECRETS = 7;
const NM_DEVICE_STATE_REASON_SUPPLICANT_DISCONNECT = 8;
const NM_DEVICE_STATE_REASON_SUPPLICANT_CONFIG_FAILED = 9;
const NM_DEVICE_STATE_REASON_SUPPLICANT_FAILED = 10;
const NM_DEVICE_STATE_REASON_SUPPLICANT_TIMEOUT = 11;
const NM_DEVICE_STATE_REASON_PPP_START_FAILED = 12;
const NM_DEVICE_STATE_REASON_PPP_DISCONNECT = 13;
const NM_DEVICE_STATE_REASON_PPP_FAILED = 14;
const NM_DEVICE_STATE_REASON_DHCP_START_FAILED = 15;
const NM_DEVICE_STATE_REASON_DHCP_ERROR = 16;
const NM_DEVICE_STATE_REASON_DHCP_FAILED = 17;
const NM_DEVICE_STATE_REASON_SHARED_START_FAILED = 18;
const NM_DEVICE_STATE_REASON_SHARED_FAILED = 19;
const NM_DEVICE_STATE_REASON_AUTOIP_START_FAILED = 20;
const NM_DEVICE_STATE_REASON_AUTOIP_ERROR = 21;
const NM_DEVICE_STATE_REASON_AUTOIP_FAILED = 22;
const NM_DEVICE_STATE_REASON_MODEM_BUSY = 23;
const NM_DEVICE_STATE_REASON_MODEM_NO_DIAL_TONE = 24;
const NM_DEVICE_STATE_REASON_MODEM_NO_CARRIER = 25;
const NM_DEVICE_STATE_REASON_MODEM_DIAL_TIMEOUT = 26;
const NM_DEVICE_STATE_REASON_MODEM_DIAL_FAILED = 27;
const NM_DEVICE_STATE_REASON_MODEM_INIT_FAILED = 28;
const NM_DEVICE_STATE_REASON_GSM_APN_FAILED = 29;
const NM_DEVICE_STATE_REASON_GSM_REGISTRATION_NOT_SEARCHING = 30;
const NM_DEVICE_STATE_REASON_GSM_REGISTRATION_DENIED = 31;
const NM_DEVICE_STATE_REASON_GSM_REGISTRATION_TIMEOUT = 32;
const NM_DEVICE_STATE_REASON_GSM_REGISTRATION_FAILED = 33;
const NM_DEVICE_STATE_REASON_GSM_PIN_CHECK_FAILED = 34;
const NM_DEVICE_STATE_REASON_FIRMWARE_MISSING = 35;
const NM_DEVICE_STATE_REASON_REMOVED = 36;
const NM_DEVICE_STATE_REASON_SLEEPING = 37;
const NM_DEVICE_STATE_REASON_CONNECTION_REMOVED = 38;
const NM_DEVICE_STATE_REASON_USER_REQUESTED = 39;
const NM_DEVICE_STATE_REASON_CARRIER = 40;


// NM_DEVICE_CAP in org.freedesktop.NetworkManager.Device
//   NONE: Null capability.
//   NM_SUPPORTED: The device is supported by NetworkManager.
//   CARRIER_DETECT: The device supports carrier detection.

const NM_DEVICE_CAP_NONE = 0x0;
const NM_DEVICE_CAP_NM_SUPPORTED = 0x1;
const NM_DEVICE_CAP_CARRIER_DETECT = 0x2;


var NetworkManagerDHCP4Config = {
    name: 'org.freedesktop.NetworkManager.DHCP4Config',
    methods: [],
    signals: [

        // PropertiesChanged:
        // In Args:
        //   properties: (String_Variant_Map) A dictionary mapping property
        // names to variant boxed values

        { name: 'PropertiesChanged', inSignature: 'a{sv}', outSignature: '' }
    ],
    properties: [

        // Options: Configuration options returned by a DHCP server, if any.

        { name: 'Options', signature: 'a{sv}', access: 'read' }
    ]
};

var NetworkManagerSettingsConnection = {
    name: 'org.freedesktop.NetworkManagerSettings.Connection',
    methods: [

        // Update: Update the connection with new settings and properties,
        // replacing all previous settings and properties.
        // In Args:
        //   properties: New connection properties.

        { name: 'Update', inSignature: 'a{sa{sv}}', outSignature: '' },

        // Delete: Delete the connection.

        { name: 'Delete', inSignature: '', outSignature: '' },

        // GetSettings: Get the settings maps describing this object.
        // Out Args:
        //   settings: (String_String_Variant_Map_Map) The nested settings
        // maps describing this object.

        { name: 'GetSettings', inSignature: '', outSignature: 'a{sa{sv}}' }
    ],
    signals: [

        // Updated: Emitted when some settings changed.
        // In Args:
        //   settings: (String_String_Variant_Map_Map) Contains the changed
        // settings.

        { name: 'Updated', inSignature: 'a{sa{sv}}', outSignature: '' },

        // Removed: Emitted when this settings object was removed. FIXME: Is
        // this emitted after it was removed or when it is about to be
        // removed? If after removed, then different semantics to Device
        // removed. if prior to removal, should be called AboutToBeRemoved.

        { name: 'Removed', inSignature: '', outSignature: '' }
    ],
    properties: []
};

var NetworkManagerSettingsConnectionSecrets = {
    name: 'org.freedesktop.NetworkManagerSettings.Connection.Secrets',
    methods: [

        // GetSecrets: Get the secrets encapsulated in this object.
        // In Args:
        //   setting_name: Name of the setting to return.
        //   hints: Array of strings of key names in the Setting for which NM
        // thinks a secrets may be required.
        //   request_new: Indicates whether new secrets should be requested or
        // if the request can be fulfilled from storage.
        // Out Args:
        //   secrets: (String_String_Variant_Map_Map) Nested settings maps
        // containing secrets. Each setting MUST contain at least the 'name'
        // field, containing the name of the setting, and one or more secrets.

        { name: 'GetSecrets', inSignature: 'sasb', outSignature: 'a{sa{sv}}' }
    ],
    signals: [],
    properties: []
};

var NetworkManagerIP4Config = {
    name: 'org.freedesktop.NetworkManager.IP4Config',
    methods: [],
    signals: [],
    properties: [

        // Addresses: Tuples of IPv4 address/prefix/gateway.

        { name: 'Addresses', signature: 'aau', access: 'read' },

        // Nameservers: The nameservers in use.

        { name: 'Nameservers', signature: 'au', access: 'read' },

        // WinsServers: The Windows Internet Name Service servers associated
        // with the connection.

        { name: 'WinsServers', signature: 'au', access: 'read' },

        // Domains: A list of domains this address belongs to.

        { name: 'Domains', signature: 'as', access: 'read' },

        // Routes: Tuples of IPv4 route/prefix/next-hop/metric.

        { name: 'Routes', signature: 'aau', access: 'read' }
    ]
};

var NetworkManager = {
    name: 'org.freedesktop.NetworkManager',
    methods: [

        // GetDevices: Get the list of network devices.
        // Out Args:
        //   devices: List of object paths of network devices known to the
        // system.

        { name: 'GetDevices', inSignature: '', outSignature: 'ao' },

        // ActivateConnection: Activate a connection using the supplied
        // device.
        // In Args:
        //   service_name: The D-Bus service name of the settings service that
        // provides this connection.
        //   connection: The connection to activate the devices with.
        //   device: The device to be activated.
        //   specific_object: The path of a device-type-specific object this
        // activation should use, for example a WiFi access point.
        // Out Args:
        //   active_connection: The path of the active connection object
        // representing this active connection.
        // Errors:
        //   org.freedesktop.NetworkManager.Error.UnknownConnection
        //   org.freedesktop.NetworkManager.Error.UnknownDevice
        //   org.freedesktop.NetworkManager.Error.InvalidService
        //   org.freedesktop.NetworkManager.Error.ConnectionActivating Another
        // connection is already activating or the same connection is already
        // active. FIXME: check if the error name is correct. FIXME: split
        // into 2 errors?
        //   org.freedesktop.NetworkManager.Error.ConnectionInvalid The
        // connection is invalid for this device.

        { name: 'ActivateConnection', inSignature: 'sooo', outSignature: 'o' },

        // DeactivateConnection: Deactivate an active connection.
        // In Args:
        //   active_connection: The currently active connection to deactivate.

        { name: 'DeactivateConnection', inSignature: 'o', outSignature: '' },

        // Sleep: Control the NetworkManager daemons sleep state. When asleep,
        // all interfaces that it manages are deactivated. When awake, devices
        // are available to be activated.
        // In Args:
        //   sleep: Indicates whether the NetworkManager daemon should sleep
        // or wake.

        { name: 'Sleep', inSignature: 'b', outSignature: '' },

        // sleep: DEPRECATED. Control the NetworkManager daemons sleep state.
        // When asleep, all interfaces that it manages are deactivated.

        { name: 'sleep', inSignature: '', outSignature: '' },

        // wake: DEPRECATED. Control the NetworkManager daemons sleep state.
        // When awake, all known interfaces are available to be activated.

        { name: 'wake', inSignature: '', outSignature: '' },

        // state: DEPRECATED. The overall state of the NetworkManager daemon.
        // Out Args:
        //   state: (NM_STATE)

        { name: 'state', inSignature: '', outSignature: 'u' }
    ],
    signals: [

        // StateChanged: NetworkManagers state changed.
        // In Args:
        //   state: (NM_STATE) The new state of NetworkManager.

        { name: 'StateChanged', inSignature: 'u', outSignature: '' },

        // PropertiesChanged: NetworkManagers properties changed.
        // In Args:
        //   properties: (String_Variant_Map) The changed properties.

        { name: 'PropertiesChanged', inSignature: 'a{sv}', outSignature: '' },

        // DeviceAdded: A device was added to the system
        // In Args:
        //   device_path: The object path of the newly added device.

        { name: 'DeviceAdded', inSignature: 'o', outSignature: '' },

        // DeviceRemoved: A device was removed from the system, and is no
        // longer available.
        // In Args:
        //   device_path: The object path of the device that was just removed.

        { name: 'DeviceRemoved', inSignature: 'o', outSignature: '' },

        // StateChange: DEPRECATED. NetworkManagers state changed. Use the
        // StateChanged signal instead.
        // In Args:
        //   state: (NM_STATE) The new state of NetworkManager.

        { name: 'StateChange', inSignature: 'u', outSignature: '' }
    ],
    properties: [

        // WirelessEnabled: Indicates if wireless is currently enabled or not.

        { name: 'WirelessEnabled', signature: 'b', access: 'readwrite' },

        // WirelessHardwareEnabled: Indicates if the wireless hardware is
        // currently enabled, i.e. the state of the RF kill switch.

        { name: 'WirelessHardwareEnabled', signature: 'b', access: 'read' },

        // ActiveConnections: List of active connection object paths.

        { name: 'ActiveConnections', signature: 'ao', access: 'read' },

        // State: (NM_STATE) The overall state of the NetworkManager daemon.

        { name: 'State', signature: 'u', access: 'read' }
    ]
};

// NM_STATE in org.freedesktop.NetworkManager
//   UNKNOWN: The NetworkManager daemon is in an unknown state.
//   ASLEEP: The NetworkManager daemon is asleep and all interfaces
// managed by it are inactive.
//   CONNECTING: The NetworkManager daemon is connecting a device.
// FIXME: What does this mean when one device is active and another is
// connecting?
//   CONNECTED: The NetworkManager daemon is connected.
//   DISCONNECTED: The NetworkManager daemon is disconnected.

const NM_STATE_UNKNOWN = 0;
const NM_STATE_ASLEEP = 1;
const NM_STATE_CONNECTING = 2;
const NM_STATE_CONNECTED = 3;
const NM_STATE_DISCONNECTED = 4;


var NetworkManagerPPP = {
    name: 'org.freedesktop.NetworkManager.PPP',
    methods: [

        // NeedSecrets:
        // Out Args:
        //   username:
        //   password:

        { name: 'NeedSecrets', inSignature: '', outSignature: 'ss' },

        // SetIp4Config:
        // In Args:
        //   config:

        { name: 'SetIp4Config', inSignature: 'a{sv}', outSignature: '' },

        // SetState:
        // In Args:
        //   state:

        { name: 'SetState', inSignature: 'u', outSignature: '' }
    ],
    signals: [],
    properties: []
};

var NetworkManagerSettingsSystem = {
    name: 'org.freedesktop.NetworkManagerSettings.System',
    methods: [

        // AddConnection: Add new connection.
        // In Args:
        //   connection: Connection properties.

        { name: 'AddConnection', inSignature: 'a{sa{sv}}', outSignature: '' },

        // SaveHostname: Save the hostname to persistent configuration.
        // In Args:
        //   hostname: The hostname to save to persistent configuration. If
        // blank, the persistent hostname is cleared.

        { name: 'SaveHostname', inSignature: 's', outSignature: '' }
    ],
    signals: [

        // PropertiesChanged:
        // In Args:
        //   properties: (String_Variant_Map) A dictionary mapping property
        // names to variant boxed values

        { name: 'PropertiesChanged', inSignature: 'a{sv}', outSignature: '' }
    ],
    properties: [

        // Hostname: The machine hostname stored in persistent configuration.

        { name: 'Hostname', signature: 's', access: 'read' },

        // CanModify: If true, adding and modifying connections is supported.

        { name: 'CanModify', signature: 'b', access: 'read' },

        // UnmanagedDevices: The list of HAL UDIs of devices that should not
        // be managed by NetworkManager.

        { name: 'UnmanagedDevices', signature: 'ao', access: 'read' }
    ]
};

var NetworkManagerSettings = {
    name: 'org.freedesktop.NetworkManagerSettings',
    methods: [

        // ListConnections: List the connections stored by this Settings
        // object.
        // Out Args:
        //   connections: List of connections.

        { name: 'ListConnections', inSignature: '', outSignature: 'ao' }
    ],
    signals: [

        // NewConnection: Emitted when a new connection has been configured.
        // In Args:
        //   connection: Object path of the new connection.

        { name: 'NewConnection', inSignature: 'o', outSignature: '' }
    ],
    properties: []
};

var NetworkManagerVPNConnection = {
    name: 'org.freedesktop.NetworkManager.VPN.Connection',
    methods: [],
    signals: [

        // PropertiesChanged:
        // In Args:
        //   properties: (String_Variant_Map) A dictionary mapping property
        // names to variant boxed values

        { name: 'PropertiesChanged', inSignature: 'a{sv}', outSignature: '' },

        // VpnStateChanged: Emitted when the state of the VPN connection has
        // changed.
        // In Args:
        //   state: (NM_VPN_CONNECTION_STATE) The new state of the VPN
        // connection.
        //   reason: (NM_VPN_CONNECTION_STATE_REASON) Reason code describing
        // the change to the new state.

        { name: 'VpnStateChanged', inSignature: 'uu', outSignature: '' }
    ],
    properties: [

        // VpnState: (NM_VPN_CONNECTION_STATE) The VPN-specific state of the
        // connection.

        { name: 'VpnState', signature: 'u', access: 'read' },

        // Banner: The banner string of the VPN connection.

        { name: 'Banner', signature: 's', access: 'read' }
    ]
};

// NM_VPN_CONNECTION_STATE in org.freedesktop.NetworkManager.VPN.Connection
//   UNKNOWN: The state of the VPN connection is unknown.
//   PREPARE: The VPN connection is preparing to connect.
//   NEED_AUTH: The VPN connection needs authorization credentials.
//   CONNECT: The VPN connection is being established. FIXME: Should
// be CONNECTING or CONNECTED.
//   IP_CONFIG_GET: The VPN connection is getting an IP address.
// FIXME: Should be an -ING
//   ACTIVATED: The VPN connection is active.
//   FAILED: The VPN connection failed.
//   DISCONNECTED: The VPN connection is disconnected.

const NM_VPN_CONNECTION_STATE_UNKNOWN = 0;
const NM_VPN_CONNECTION_STATE_PREPARE = 1;
const NM_VPN_CONNECTION_STATE_NEED_AUTH = 2;
const NM_VPN_CONNECTION_STATE_CONNECT = 3;
const NM_VPN_CONNECTION_STATE_IP_CONFIG_GET = 4;
const NM_VPN_CONNECTION_STATE_ACTIVATED = 5;
const NM_VPN_CONNECTION_STATE_FAILED = 6;
const NM_VPN_CONNECTION_STATE_DISCONNECTED = 7;


// NM_VPN_CONNECTION_STATE_REASON in org.freedesktop.NetworkManager.VPN.Connection
//   UNKNOWN: The reason for the VPN connection state change is
// unknown.
//   NONE: No reason was given for the VPN connection state change.
//   USER_DISCONNECTED: The VPN connection changed state because the
// user disconnected it.
//   DEVICE_DISCONNECTED: The VPN connection changed state because the
// device it was using was disconnected.
//   SERVICE_STOPPED: The service providing the VPN connection was
// stopped.
//   IP_CONFIG_INVALID: The IP config of the VPN connection was
// invalid.
//   CONNECT_TIMEOUT: The connection attempt to the VPN service timed
// out.
//   SERVICE_START_TIMEOUT: A timeout occurred while starting the
// service providing the VPN connection.
//   SERVICE_START_FAILED: Starting the service starting the service
// providing the VPN connection failed.
//   NO_SECRETS: Necessary secrets for the VPN connection were not
// provided.

const NM_VPN_CONNECTION_STATE_REASON_UNKNOWN = 0;
const NM_VPN_CONNECTION_STATE_REASON_NONE = 1;
const NM_VPN_CONNECTION_STATE_REASON_USER_DISCONNECTED = 2;
const NM_VPN_CONNECTION_STATE_REASON_DEVICE_DISCONNECTED = 3;
const NM_VPN_CONNECTION_STATE_REASON_SERVICE_STOPPED = 4;
const NM_VPN_CONNECTION_STATE_REASON_IP_CONFIG_INVALID = 5;
const NM_VPN_CONNECTION_STATE_REASON_CONNECT_TIMEOUT = 6;
const NM_VPN_CONNECTION_STATE_REASON_SERVICE_START_TIMEOUT = 7;
const NM_VPN_CONNECTION_STATE_REASON_SERVICE_START_FAILED = 8;
const NM_VPN_CONNECTION_STATE_REASON_NO_SECRETS = 9;


var NetworkManagerVPNPlugin = {
    name: 'org.freedesktop.NetworkManager.VPN.Plugin',
    methods: [

        // Connect: Tells the plugin to connect.
        // In Args:
        //   connection: (String_String_Variant_Map_Map) Describes the
        // connection to be established.
        // Errors:
        //   org.freedesktop.NetworkManager.VPN.Error.StartingInProgress
        //   org.freedesktop.NetworkManager.VPN.Error.AlreadyStarted
        //   org.freedesktop.NetworkManager.VPN.Error.StoppingInProgress
        //   org.freedesktop.NetworkManager.VPN.Error.BadArguments
        //   org.freedesktop.NetworkManager.VPN.Error.LaunchFailed

        { name: 'Connect', inSignature: 'a{sa{sv}}', outSignature: '' },

        // NeedSecrets: Asks the plugin whether the provided connection will
        // require secrets to connect successfully.
        // In Args:
        //   settings: (String_String_Variant_Map_Map) Describes the
        // connection that may need secrets.
        // Out Args:
        //   setting_name: The setting name within the provided connection
        // that requires secrets, if any.
        // Errors:
        //   org.freedesktop.NetworkManager.VPN.Error.ConnectionInvalid

        { name: 'NeedSecrets', inSignature: 'a{sa{sv}}', outSignature: 's' },

        // Disconnect: Disconnect the plugin.
        // Errors:
        //   org.freedesktop.NetworkManager.VPN.Error.StoppingInProgress
        //   org.freedesktop.NetworkManager.VPN.Error.AlreadyStopped

        { name: 'Disconnect', inSignature: '', outSignature: '' },

        // SetIp4Config: Set IPv4 details on the connection.
        // In Args:
        //   config: (String_Variant_Map) Ip4Config details for the
        // conneciton.

        { name: 'SetIp4Config', inSignature: 'a{sv}', outSignature: '' },

        // SetFailure: Indicate a failure to the plugin.
        // In Args:
        //   reason: The reason for the failure.

        { name: 'SetFailure', inSignature: 's', outSignature: '' }
    ],
    signals: [

        // StateChanged: Emitted when the plugin state changes.
        // In Args:
        //   state: (NM_VPN_CONNECTION_STATE) The new state of the plugin.

        { name: 'StateChanged', inSignature: 'u', outSignature: '' },

        // Ip4Config: The plugin obtained an IPv4 configuration.
        // In Args:
        //   ip4config: (String_Variant_Map) The IPv4 configuration.

        { name: 'Ip4Config', inSignature: 'a{sv}', outSignature: '' },

        // LoginBanner: Emitted when the plugin receives a login banner from
        // the VPN service.
        // In Args:
        //   banner: The login banner string.

        { name: 'LoginBanner', inSignature: 's', outSignature: '' },

        // Failure: Emitted when a failure in the VPN plugin occurs.
        // In Args:
        //   reason: (NM_VPN_PLUGIN_FAILURE) Reason code for the failure.

        { name: 'Failure', inSignature: 'u', outSignature: '' }
    ],
    properties: [

        // State: (NM_VPN_CONNECTION_STATE) The state of the plugin.

        { name: 'State', signature: 'u', access: 'read' }
    ]
};

// NM_VPN_PLUGIN_FAILURE in org.freedesktop.NetworkManager.VPN.Plugin
//   LOGIN_FAILED: Login failed.
//   CONNECT_FAILED: Connect failed.
//   BAD_IP_CONFIG: Invalid IP configuration returned from the VPN
// plugin.

const NM_VPN_PLUGIN_FAILURE_LOGIN_FAILED = 0;
const NM_VPN_PLUGIN_FAILURE_CONNECT_FAILED = 1;
const NM_VPN_PLUGIN_FAILURE_BAD_IP_CONFIG = 2;

