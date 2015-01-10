//  --------------------------------------------------------------------------------------------------------------------
//  <copyright file="MainViewModel.cs" company="EvePanix">Copyright (c) Jedzia 2001-2014, EvePanix. All rights reserved. See the license notes shipped with this source and the GNU GPL.</copyright>
//  <author>Jedzia</author>
//  <email>jed69@gmx.de</email>
//  <date>10.01.2015 19:11</date>
//  --------------------------------------------------------------------------------------------------------------------

namespace Jedzia.Arduino.Chaoslader.Application.ViewModel
{
    using System.Threading;

    using Castle.Core.Logging;

    using GalaSoft.MvvmLight;

    using Jedzia.Arduino.Chaoslader.Application.Model;

    /// <summary>
    ///     This class contains properties that the main View can data bind to.
    ///     <para>
    ///         See http://www.galasoft.ch/mvvm
    ///     </para>
    /// </summary>
    public class MainViewModel : ViewModelBase
    {
        /// <summary>
        ///     The <see cref="WelcomeTitle" /> property's name.
        /// </summary>
        public const string WelcomeTitlePropertyName = "WelcomeTitle";

        public const string TestPropertyName = "Test";

        private readonly IDataService dataService;

        private readonly ILogger logger;

        private readonly Timer t;

        private string test = "Haha";

        private string welcomeTitle = string.Empty;

        /// <summary>
        ///     Initializes a new instance of the MainViewModel class.
        /// </summary>
        public MainViewModel(IDataService dataService, ILogger logger)
        {
            this.dataService = dataService;
            this.logger = logger;
            this.dataService.GetData(
                (item, error) =>
                {
                    if (error != null)
                    {
                        // Report error here
                        return;
                    }

                    this.WelcomeTitle = item.Title;
                });

            logger.Warn("This is the first Message from MainViewModel");

            //this.t = new Timer(this.Callback);
            // this.t.Change(1000, 1000);
        }

        /// <summary>
        ///     Gets the WelcomeTitle property.
        ///     Changes to that property's value raise the PropertyChanged event.
        /// </summary>
        public string WelcomeTitle
        {
            get
            {
                return this.welcomeTitle;
            }

            set
            {
                if (this.welcomeTitle == value)
                {
                    return;
                }

                this.welcomeTitle = value;
                this.RaisePropertyChanged(WelcomeTitlePropertyName);
            }
        }

        public string Test
        {
            get
            {
                return this.test;
            }

            set
            {
                if (this.test == value)
                {
                    return;
                }

                this.test = value;
                this.RaisePropertyChanged(TestPropertyName);
                this.logger.Warn("" + TestPropertyName + " Changed to '" + value + "'");
            }
        }

        private void Callback(object state)
        {
            this.logger.Warn("A Callback Message from MainViewModel");
        }

        public override void Cleanup()
        {
            // Clean up if needed

            base.Cleanup();
            this.t.Dispose();
        }
    }
}