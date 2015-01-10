using System.Windows;
using Jedzia.Arduino.Chaoslader.Application.ViewModel;

namespace Jedzia.Arduino.Chaoslader.Application
{
    using System;

    using Castle.Core.Logging;

    using Jedzia.Arduino.Chaoslader.Application.Helper;

    using NLog;
    using NLog.Config;
    using NLog.Targets;
    using NLog.Targets.Wrappers;

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        /// <summary>
        /// Initializes a new instance of the MainWindow class.
        /// </summary>
        public MainWindow()
        {
            InitializeComponent();
            Closing += (s, e) => ViewModelLocator.Cleanup();
        }

        private const string ConsoleTargetName = "WpfConsole";
        private Logger _logger;
        private AsyncTargetWrapper _wrapper;
        private ILogger Logger
        {
            //get { return _logger ?? (_logger = LogManager.GetLogger(GetType().Name)); }
            get { return ViewModelLocator.GetLogger(); }
        }

        private void MainWindowInitialized(object sender, System.EventArgs e)
        {
            // Set Log Target
            // https://nlog.codeplex.com/workitem/6272
            /*var target = new WpfRichTextBoxTarget
            {
                Name = ConsoleTargetName,
                Layout = "${time:}|${threadid:padding=3}|${level:uppercase=true:padding=-5}|${logger:padding=-15}|${message}|${exception}",
                ControlName = TextLog.Name,
                FormName = Name,
                AutoScroll = true,
                MaxLines = 100000,
                UseDefaultRowColoringRules = true
            };
            _wrapper = new AsyncTargetWrapper
            {
                Name = ConsoleTargetName,
                WrappedTarget = target
            };*/
            //SimpleConfigurator.ConfigureForTargetLogging(_wrapper, LogLevel.Trace);
            TextLog.Document.PageWidth = 1000;

            Dispatcher.Invoke(() =>
            {
                var target = new WpfRichTextBoxTarget
                {
                    Name = "RichText",
                    Layout =
                        //"[${longdate:useUTC=false}] :: [${level:uppercase=true}] :: ${logger}:${callsite} :: ${message} ${exception:innerFormat=tostring:maxInnerExceptionLevel=10:separator=,:format=tostring}",
                    "${date:format=MM-dd-yyyy HH\\:mm\\:ss} [${uppercase:${level}}] ${message}",
                    ControlName = TextLog.Name,
                    FormName = GetType().Name,
                    AutoScroll = true,
                    MaxLines = 100000,
                    UseDefaultRowColoringRules = true,
                };
                var asyncWrapper = new AsyncTargetWrapper { Name = "RichTextAsync", WrappedTarget = target };

                LogManager.Configuration.AddTarget(asyncWrapper.Name, asyncWrapper);
                LogManager.Configuration.LoggingRules.Insert(0, new LoggingRule("*", LogLevel.Debug, asyncWrapper));
                LogManager.ReconfigExistingLoggers();

            });


            /*Logger.Log(LogLevel.Info, "The time is: {0}", DateTime.Now);
            Logger.Log(LogLevel.Error, "The time is: {0}", DateTime.Now);
            Logger.Log(LogLevel.Debug, "The time is: {0}", DateTime.Now);
            Logger.Log(LogLevel.Trace, "The time is: {0}", DateTime.Now);
            Logger.Log(LogLevel.Warn, "The time is: {0}", DateTime.Now);
            //Logger.Log(LogLevel.Off, "The time is: {0}", DateTime.Now);*/

            Logger.Info(string.Format("The time is: {0}", DateTime.Now));
            Logger.Error( string.Format("The time is: {0}", DateTime.Now));
            Logger.Debug(string.Format( "The time is: {0}", DateTime.Now));
            Logger.Warn(string.Format("The time is: {0}", DateTime.Now));
            //Logger.Log(LogLevel.Off, "The time is: {0}", DateTime.Now);

        }
    }
}