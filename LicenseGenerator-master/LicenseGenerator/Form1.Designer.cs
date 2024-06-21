namespace LicenseGenerator
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.macAddress = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.expiryDate = new System.Windows.Forms.DateTimePicker();
            this.cipherText = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.generateBtn = new System.Windows.Forms.Button();
            this.exportBtn = new System.Windows.Forms.Button();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.exportText = new System.Windows.Forms.Label();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.lblDecryptMessage = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.licenseExpiryDate = new System.Windows.Forms.TextBox();
            this.licenseMacAddress = new System.Windows.Forms.TextBox();
            this.decryptButton = new System.Windows.Forms.Button();
            this.browseLicenseButton = new System.Windows.Forms.Button();
            this.licenseFileName = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(16, 21);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(76, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Email Address:";
            // 
            // macAddress
            // 
            this.macAddress.Location = new System.Drawing.Point(100, 16);
            this.macAddress.MaxLength = 0;
            this.macAddress.Name = "macAddress";
            this.macAddress.Size = new System.Drawing.Size(368, 20);
            this.macAddress.TabIndex = 1;
            this.macAddress.TextChanged += new System.EventHandler(this.macAddress_TextChanged);
            this.macAddress.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.macAddress_KeyPress);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(16, 50);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(64, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Expiry Date:";
            // 
            // expiryDate
            // 
            this.expiryDate.Location = new System.Drawing.Point(100, 46);
            this.expiryDate.Name = "expiryDate";
            this.expiryDate.Size = new System.Drawing.Size(200, 20);
            this.expiryDate.TabIndex = 3;
            this.expiryDate.ValueChanged += new System.EventHandler(this.expiryDate_ValueChanged);
            // 
            // cipherText
            // 
            this.cipherText.Location = new System.Drawing.Point(100, 76);
            this.cipherText.Name = "cipherText";
            this.cipherText.Size = new System.Drawing.Size(368, 20);
            this.cipherText.TabIndex = 4;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(40, 79);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(43, 13);
            this.label3.TabIndex = 5;
            this.label3.Text = "Cipher: ";
            // 
            // generateBtn
            // 
            this.generateBtn.Location = new System.Drawing.Point(100, 105);
            this.generateBtn.Name = "generateBtn";
            this.generateBtn.Size = new System.Drawing.Size(75, 25);
            this.generateBtn.TabIndex = 6;
            this.generateBtn.Text = "Generate";
            this.generateBtn.UseVisualStyleBackColor = true;
            this.generateBtn.Click += new System.EventHandler(this.generateBtn_Click);
            // 
            // exportBtn
            // 
            this.exportBtn.Location = new System.Drawing.Point(181, 105);
            this.exportBtn.Name = "exportBtn";
            this.exportBtn.Size = new System.Drawing.Size(75, 25);
            this.exportBtn.TabIndex = 7;
            this.exportBtn.Text = "Export";
            this.exportBtn.UseVisualStyleBackColor = true;
            this.exportBtn.Click += new System.EventHandler(this.exportBtn_Click);
            // 
            // saveFileDialog1
            // 
            this.saveFileDialog1.FileName = "license.dat";
            this.saveFileDialog1.FileOk += new System.ComponentModel.CancelEventHandler(this.saveFileDialog1_FileOk);
            // 
            // exportText
            // 
            this.exportText.AutoSize = true;
            this.exportText.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.exportText.Location = new System.Drawing.Point(98, 133);
            this.exportText.Name = "exportText";
            this.exportText.Size = new System.Drawing.Size(35, 13);
            this.exportText.TabIndex = 8;
            this.exportText.Text = "label4";
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(729, 507);
            this.tabControl1.TabIndex = 9;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.label1);
            this.tabPage1.Controls.Add(this.exportText);
            this.tabPage1.Controls.Add(this.macAddress);
            this.tabPage1.Controls.Add(this.exportBtn);
            this.tabPage1.Controls.Add(this.label2);
            this.tabPage1.Controls.Add(this.generateBtn);
            this.tabPage1.Controls.Add(this.expiryDate);
            this.tabPage1.Controls.Add(this.label3);
            this.tabPage1.Controls.Add(this.cipherText);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(721, 481);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Encrypt";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.lblDecryptMessage);
            this.tabPage2.Controls.Add(this.label6);
            this.tabPage2.Controls.Add(this.label5);
            this.tabPage2.Controls.Add(this.licenseExpiryDate);
            this.tabPage2.Controls.Add(this.licenseMacAddress);
            this.tabPage2.Controls.Add(this.decryptButton);
            this.tabPage2.Controls.Add(this.browseLicenseButton);
            this.tabPage2.Controls.Add(this.licenseFileName);
            this.tabPage2.Controls.Add(this.label4);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(721, 481);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Decrypt";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // lblDecryptMessage
            // 
            this.lblDecryptMessage.AutoSize = true;
            this.lblDecryptMessage.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.lblDecryptMessage.Location = new System.Drawing.Point(186, 53);
            this.lblDecryptMessage.Name = "lblDecryptMessage";
            this.lblDecryptMessage.Size = new System.Drawing.Size(35, 13);
            this.lblDecryptMessage.TabIndex = 17;
            this.lblDecryptMessage.Text = "label7";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(22, 113);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(64, 13);
            this.label6.TabIndex = 16;
            this.label6.Text = "Expiry Date:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(22, 83);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(74, 13);
            this.label5.TabIndex = 15;
            this.label5.Text = "MAC Address:";
            // 
            // licenseExpiryDate
            // 
            this.licenseExpiryDate.Location = new System.Drawing.Point(105, 108);
            this.licenseExpiryDate.Name = "licenseExpiryDate";
            this.licenseExpiryDate.ReadOnly = true;
            this.licenseExpiryDate.Size = new System.Drawing.Size(368, 20);
            this.licenseExpiryDate.TabIndex = 14;
            // 
            // licenseMacAddress
            // 
            this.licenseMacAddress.Location = new System.Drawing.Point(105, 79);
            this.licenseMacAddress.Name = "licenseMacAddress";
            this.licenseMacAddress.ReadOnly = true;
            this.licenseMacAddress.Size = new System.Drawing.Size(368, 20);
            this.licenseMacAddress.TabIndex = 13;
            // 
            // decryptButton
            // 
            this.decryptButton.Location = new System.Drawing.Point(105, 48);
            this.decryptButton.Name = "decryptButton";
            this.decryptButton.Size = new System.Drawing.Size(75, 25);
            this.decryptButton.TabIndex = 12;
            this.decryptButton.Text = "Decrypt";
            this.decryptButton.UseVisualStyleBackColor = true;
            this.decryptButton.Click += new System.EventHandler(this.decryptButton_Click);
            // 
            // browseLicenseButton
            // 
            this.browseLicenseButton.Location = new System.Drawing.Point(480, 16);
            this.browseLicenseButton.Name = "browseLicenseButton";
            this.browseLicenseButton.Size = new System.Drawing.Size(75, 25);
            this.browseLicenseButton.TabIndex = 11;
            this.browseLicenseButton.Text = "Browse";
            this.browseLicenseButton.UseVisualStyleBackColor = true;
            this.browseLicenseButton.Click += new System.EventHandler(this.browseLicenseButton_Click);
            // 
            // licenseFileName
            // 
            this.licenseFileName.Location = new System.Drawing.Point(105, 17);
            this.licenseFileName.Name = "licenseFileName";
            this.licenseFileName.ReadOnly = true;
            this.licenseFileName.Size = new System.Drawing.Size(368, 20);
            this.licenseFileName.TabIndex = 10;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(16, 21);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(66, 13);
            this.label4.TabIndex = 0;
            this.label4.Text = "License File:";
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "license.dat";
            this.openFileDialog1.InitialDirectory = "C:\\";
            this.openFileDialog1.FileOk += new System.ComponentModel.CancelEventHandler(this.openFileDialog1_FileOk);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(729, 507);
            this.Controls.Add(this.tabControl1);
            this.Name = "Form1";
            this.Text = "License Generator";
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox macAddress;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.DateTimePicker expiryDate;
        private System.Windows.Forms.TextBox cipherText;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button generateBtn;
        private System.Windows.Forms.Button exportBtn;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        private System.Windows.Forms.Label exportText;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.Button browseLicenseButton;
        private System.Windows.Forms.TextBox licenseFileName;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox licenseExpiryDate;
        private System.Windows.Forms.TextBox licenseMacAddress;
        private System.Windows.Forms.Button decryptButton;
        private System.Windows.Forms.Label lblDecryptMessage;
    }
}

