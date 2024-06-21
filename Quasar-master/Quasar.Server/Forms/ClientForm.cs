using System;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Windows.Forms;
using Quasar.Server.ViewModel;

namespace Quasar.Server.Forms
{
    public partial class ClientForm : Form
    {
        private readonly ClientModel _model = new ClientModel();

        public ClientForm()
        {
            InitializeComponent();
            BindData();
        }

        /// <summary>
        /// Parameter binding
        /// </summary>
        private void BindData()
        {
            tbEmail.DataBindings.Add("Text", _model, nameof(_model.Email));
            // get mac address
            //_model.GetMACAddress();
            //tbMAC.DataBindings.Add("Text", _model, nameof(_model.MACAddress));
            // Example public key
            _model.PublicKey = @"-----BEGIN PUBLIC KEY----- MIGeMA0GCSqGSIb3DQEBAQUAA4GMADCBiAKBgGFPnrvYFsHG3+NAFcVf4czqpdFX Of/eQyyFTUxwm4qjPJGLpm/agh5U3gUS6E5t9QHHSpN6hf3g8qIMgblDtTSltU4r mWEf3C8JoHK9fSsJeo2JadOSoJj8YBTPFjOTNz7/PkS0F+Sn/8to/ybzt8tUReT9 5Fxi4JWkJyxQpcWnAgMBAAE= -----END PUBLIC KEY-----";
            //tbPublicKey.DataBindings.Add("Text", _model, nameof(_model.PublicKey));
            tbLicense.DataBindings.Add("Text", _model, nameof(_model.License));
        }

        private async void BtnActivate_Click(object sender, EventArgs e)
        {
            if (_model.ActivateLicense())
            {
                this.DialogResult = DialogResult.OK;
                /*
                using (var dialogForm = new FrmMain())
                {
                    // Initialize your dialog form if needed
                    // ...

                    await ShowFormAsync(dialogForm);

                    DialogResult result = dialogForm.ShowDialog();
                    Debug.Print($"ShowDialog result {result}");

                    // Show the dialog using ShowDialog
                    if (result == DialogResult.OK)
                    {
                        Debug.Print("FrmMain Open");
                        // Handle the result if necessary
                        this.Close();
                    }
                }
                */
            }
            else
            {
                this.DialogResult = DialogResult.Cancel;
            }

            this.Close();
        }

        private async Task ShowFormAsync(Form form)
        {
            // Use TaskCompletionSource to await the form closure
            var tcs = new TaskCompletionSource<object>();

            // Hook the FormClosed event to signal completion
            form.FormClosed += (sender, e) => tcs.TrySetResult(null);

            // Show the form asynchronously
            form.Show();
            
            this.Hide();

            // Await the form closure
            await tcs.Task;
        }
    }
}

//1@gmail.com
//5y2u4d9oxg2OqHQv3b/DgQ7aKSunbPnyKSlaSIHVqQI=