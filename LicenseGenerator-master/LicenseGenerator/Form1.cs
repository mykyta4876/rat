using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Security.Cryptography;

namespace LicenseGenerator
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            cipherText.Text = "";
            exportText.Text = "";
            licenseFileName.Text = "";
            licenseMacAddress.Text = "";
            licenseExpiryDate.Text = "";
            decryptButton.Enabled = false;
            cipherText.ReadOnly = true;
            exportBtn.Enabled = false;
            lblDecryptMessage.Text = "";
        }

        private void generateBtn_Click(object sender, EventArgs e)
        {
            /*
            if (macAddress.Text.Length != 12)
            {
                MessageBox.Show("Invalid MAC Address.");
                return;
            }
            */

            Crypto cryto = new Crypto();

            byte[] bytesToBeEncrypted = Encoding.UTF8.GetBytes(macAddress.Text + "-" + expiryDate.Value.ToString("ddMMyyyy"));
            byte[] passwordBytes = Encoding.UTF8.GetBytes("Lorentz@QWESTRO");

            passwordBytes = SHA256.Create().ComputeHash(passwordBytes);
            byte[] bytesEncrypted = cryto.AES_Encrypt(bytesToBeEncrypted, passwordBytes);
            cipherText.Text = Convert.ToBase64String(bytesEncrypted);
            exportBtn.Enabled = true;
        }

        private void macAddress_TextChanged(object sender, EventArgs e)
        {
            cipherText.Text = "";
            exportText.Text = "";
            exportBtn.Enabled = false;
        }

        private void expiryDate_ValueChanged(object sender, EventArgs e)
        {
            cipherText.Text = "";
            exportText.Text = "";
            exportBtn.Enabled = false;
        }

        private void exportBtn_Click(object sender, EventArgs e)
        {
            saveFileDialog1.ShowDialog();
        }

        private void saveFileDialog1_FileOk(object sender, CancelEventArgs e)
        {
            string fileName = saveFileDialog1.FileName;

            try {
                File.WriteAllText(fileName, cipherText.Text);
                exportText.Text = "File Exported";
            } catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void macAddress_KeyPress(object sender, KeyPressEventArgs e)
        {
            /*
            if (Char.IsLetterOrDigit(e.KeyChar) || e.KeyChar == '\b')
            {
                e.Handled = false;
            }
            else
            {
                e.Handled = true;
            }
            */
        }

        private void openFileDialog1_FileOk(object sender, CancelEventArgs e)
        {
            licenseFileName.Text = "";
            licenseExpiryDate.Text = "";
            licenseMacAddress.Text = "";
            lblDecryptMessage.Text = "";

            string fileName = openFileDialog1.FileName;
            if (fileName != null || fileName != "")
            {
                licenseFileName.Text = fileName;
                decryptButton.Enabled = false;
            }

            decryptButton.Enabled = true;
        }

        private void browseLicenseButton_Click(object sender, EventArgs e)
        {
            openFileDialog1.ShowDialog();
        }

        private void decryptButton_Click(object sender, EventArgs e)
        {
            if (licenseFileName.Text == "")
            {
                lblDecryptMessage.Text = "Choose license file to decrypt.";
                decryptButton.Enabled = false;
            }

            try
            {
                string[] encryptedText = File.ReadAllLines(licenseFileName.Text);
                Crypto cryptoHelper = new Crypto();

                byte[] encryptedBytes = Convert.FromBase64String(encryptedText[0]);
                byte[] passwordBytes = Encoding.UTF8.GetBytes("Lorentz@QWESTRO");
                passwordBytes = SHA256.Create().ComputeHash(passwordBytes);
                byte[] decryptedBytes = cryptoHelper.AES_Decrypt(encryptedBytes, passwordBytes);
                string decryptedText = Encoding.UTF8.GetString(decryptedBytes);

                licenseMacAddress.Text = decryptedText.Substring(0, 12).ToUpper();

                DateTime expiryDate = DateTime.ParseExact(decryptedText.Substring(13), "ddMMyyyy", null);

                licenseExpiryDate.Text = expiryDate.ToString();
            } catch (Exception ex)
            {
                lblDecryptMessage.Text = ex.ToString();
            }
        }
    }
}
