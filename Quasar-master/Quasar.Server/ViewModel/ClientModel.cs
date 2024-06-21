using System;
using System.Linq;
using System.Net.NetworkInformation;
using System.Security.Cryptography;
using System.Windows.Forms;
using System.Diagnostics;
using System.Text;
using Microsoft.Win32;


namespace Quasar.Server.ViewModel
{
    public class ClientModel : ViewModel
    {
        private string _email;
        private DateTime _date;
        private string _macAddress;
        private string _publicKey;
        private string _license;

        public string Email
        {
            get => _email;
            set => SetProperty(ref _email, value);
        }

        public DateTime Date
        {
            get => _date;
            set => SetProperty(ref _date, value);
        }

        public string MACAddress
        {
            get => _macAddress;
            set => SetProperty(ref _macAddress, value);
        }

        public string PublicKey
        {
            get => _publicKey;
            set => SetProperty(ref _publicKey, value);
        }

        public string License
        {
            get => _license;
            set => SetProperty(ref _license, value);
        }

        /// <summary>
        /// Get the local MAC address
        /// </summary>
        public void GetMACAddress()
        {
            MACAddress = NetworkInterface
                .GetAllNetworkInterfaces()
                .Where(nic =>
                    nic.OperationalStatus == OperationalStatus.Up &&
                    nic.NetworkInterfaceType != NetworkInterfaceType.Loopback &&
                    nic.GetPhysicalAddress().ToString() != "")
                .Select(nic => nic.GetPhysicalAddress().ToString())
                .FirstOrDefault();

            
        }

        /// <summary>
        /// Activate license
        /// </summary>
        public bool ActivateLicense()
        {
            try
            {
                Crypto cryptoHelper = new Crypto();
                byte[] encryptedBytes = Convert.FromBase64String(License);
                byte[] passwordBytes = Encoding.UTF8.GetBytes("Lorentz@QWESTRO");
                passwordBytes = SHA256.Create().ComputeHash(passwordBytes);
                byte[] decryptedBytes = cryptoHelper.AES_Decrypt(encryptedBytes, passwordBytes);
                string decryptedText = Encoding.UTF8.GetString(decryptedBytes);

                string[] nodes = decryptedText.Split('-');

                if (nodes.Length != 2)
                {
                    MessageBox.Show($"Invalid License");
                    return false;
                }

                string licenseEmailAddress = nodes[0];

                if (! licenseEmailAddress.Equals(Email))
                {
                    Debug.Print($"Email:{Email}, licenseEmailAddress:{licenseEmailAddress}");

                    MessageBox.Show($"Invalid License");
                    return false;
                }

                DateTime expiryDate = DateTime.ParseExact(nodes[1], "ddMMyyyy", null);
                
                // Get the current date and time
                DateTime currentDateTime = DateTime.Now;

                if (expiryDate < currentDateTime)
                {
                    MessageBox.Show($"Expired License");
                    return false;
                }

                string licenseExpiryDate = expiryDate.ToString();


                // Specify the registry key and value name
                string registryKeyPath = "Software\\BlueStack";

                SetRegValue(registryKeyPath, "Email", Email);
                SetRegValue(registryKeyPath, "License", License);

                MessageBox.Show($"Activation successful, authorized to {licenseEmailAddress}，Valid until {licenseExpiryDate}");
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
                return false;
            }

            return true;
        }

        public void SetRegValue(string registryKeyPath, string valueName, string valueData)
        {
            try
            {
                // Check if the registry key exists
                if (Microsoft.Win32.Registry.CurrentUser.OpenSubKey(registryKeyPath) == null)
                {
                    // If the key doesn't exist, create it
                    using (RegistryKey key = Microsoft.Win32.Registry.CurrentUser.CreateSubKey(registryKeyPath))
                    {
                        // Add the value to the registry
                        key.SetValue(valueName, valueData);
                    }
                }
                else
                {
                    // If the key exists, open it and update the value
                    using (RegistryKey key = Microsoft.Win32.Registry.CurrentUser.OpenSubKey(registryKeyPath, true))
                    {
                        // Update the value in the registry
                        key.SetValue(valueName, valueData);
                    }
                }

                Debug.Print("Registry value added or updated successfully.");
            }
            catch (Exception ex)
            {
                Debug.Print($"Error adding or updating registry value: {ex.Message}");
            }
        }
    }
}