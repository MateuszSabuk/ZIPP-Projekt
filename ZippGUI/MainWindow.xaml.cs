using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;

using CLI;

using System.Diagnostics;
using System.Text.RegularExpressions;
using static System.Runtime.InteropServices.JavaScript.JSType;
using System.Reflection.PortableExecutable;

namespace ZippGUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private SolverManager manager;
        private int[] machines = { };
        private int[,] taskTimes = { };
        private Thread? runThread;

        public MainWindow()
        {
            manager = new SolverManager();
            InitializeComponent();
            // Set default values for the inputs
            numOfStages.Text = "4";
            numOfTasks.Text = "10";
            maxNumOfMachinesInStage.Text = "5";
            maxTaskTime.Text = "20";
        }

        private void GenerateButtonClick(object sender, RoutedEventArgs e)
        {
            if (runThread != null && runThread.IsAlive) return;
            try
            {
                int s = int.Parse(numOfStages.Text);
                int n = int.Parse(numOfTasks.Text);
                int maxMIS = int.Parse(maxNumOfMachinesInStage.Text);
                int maxTT = int.Parse(maxTaskTime.Text);
                Tuple<int[], int[,]> data = manager.generate(s, n, maxMIS, maxTT);
                GeneratedText.Text = generatedTuple2string(data);
                machines = data.Item1;
                taskTimes = data.Item2;

                // TODO display nicely
            }
            catch (Exception ex)
            {
                // TODO Exception handling
            }
        }

        private void NumberValidationTextBox(object sender, TextCompositionEventArgs e)
        {
            Regex regex = new Regex("^[0-9]+$");
            e.Handled = !regex.IsMatch(e.Text);
        }

        private void AlgorithmChoice_Loaded(object sender, EventArgs e)
        {
            string[] algorithmNames = manager.getAlgorithmNames();

            AlgorithmChoice.ItemsSource = algorithmNames;
            if (algorithmNames.Length > 0)
            {
                AlgorithmChoice.SelectedIndex = 0;
            }
        }

        private void Run_Button_Click(object sender, RoutedEventArgs e)
        {
            if (runThread != null && runThread.IsAlive) return;
            int algId = AlgorithmChoice.SelectedIndex;
            
            runThread = new Thread(new ThreadStart(() =>
            {
                Dispatcher.Invoke(() =>
                {
                    RunButton.IsEnabled = false;
                    StopButton.IsEnabled = true;
                });
                var result = manager.run(algId, machines, taskTimes);
                Dispatcher.Invoke(() =>
                {
                    AnswerText.Text = answerTuple2string(result);
                    RunButton.IsEnabled = true;
                    StopButton.IsEnabled = false;
                });
            }));
            runThread.Start();
        }



        private string generatedTuple2string(Tuple<int[], int[,]> instance)
        {
            // Extract machines and taskTimes from the Tuple
            int[] machines = instance.Item1;
            int[,] taskTimes = instance.Item2;

            // Initialize result string
            StringBuilder str = new StringBuilder();
            str.AppendLine("Instance object:");
            str.AppendLine("Stages:");

            // Convert machines (1D array) to string
            str.Append("[");
            for (int i = 0; i < machines.Length - 1; i++)
            {
                str.Append(machines[i].ToString() + ", ");
            }
            str.Append(machines[machines.Length - 1].ToString() + "]\n");

            // Convert taskTimes (2D array) to string
            str.AppendLine("Tasks:");
            for (int i = 0; i < taskTimes.GetLength(0); i++)
            {
                str.Append("[");
                for (int j = 0; j < taskTimes.GetLength(1) - 1; j++)
                {
                    str.Append(taskTimes[i, j].ToString() + ", ");
                }
                str.Append(taskTimes[i, taskTimes.GetLength(1) - 1].ToString() + "]\n");
            }

            return str.ToString();
        }
        public static string answerTuple2string(Tuple<int, int>[,] tuples)
        {
            StringBuilder sb = new StringBuilder();

            // Get the dimensions of the 2D array
            int rows = tuples.GetLength(0);
            int cols = tuples.GetLength(1);

            // Loop through the array to format each tuple
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    var tuple = tuples[i, j];
                    sb.Append($"({tuple.Item1}, {tuple.Item2})");

                    // Add a space between tuples except for the last one in the row
                    if (j < cols - 1)
                    {
                        sb.Append("  ");
                    }
                }
                // Add a new line after each row
                sb.AppendLine();
            }

            return sb.ToString();
        }

        private void PopulateAlgorithmParams(int algId)
        {
            // Get the algorithm parameters from the getAlgorithmParams method
            Dictionary<string, int> parameters = manager.getAlgorithmParams(algId);

            // Clear existing children in the AlgorithmParamsPanel
            AlgorithmParamsPanel.Children.Clear();

            foreach (var param in parameters)
            {
                // Create a StackPanel to hold the label and textbox
                StackPanel paramPanel = new StackPanel { Orientation = Orientation.Horizontal, Margin = new Thickness(0, 5, 0, 5) };

                // Create the label for the parameter
                Label label = new Label
                {
                    Content = param.Key,
                    Width = 150,
                    VerticalAlignment = VerticalAlignment.Center
                };

                // Create the TextBox for the parameter value
                TextBox textBox = new TextBox
                {
                    Width = 100,
                    Text = param.Value.ToString(),
                    Background = System.Windows.Media.Brushes.White,
                    BorderBrush = System.Windows.Media.Brushes.Gray,
                    Padding = new Thickness(5),
                    Margin = new Thickness(10, 0, 0, 0)
                };

                // Add the label and textbox to the paramPanel
                paramPanel.Children.Add(label);
                paramPanel.Children.Add(textBox);

                // Add the paramPanel to the AlgorithmParamsPanel
                AlgorithmParamsPanel.Children.Add(paramPanel);
            }
        }

        private void AlgorithmChoice_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (runThread != null && runThread.IsAlive) return;
            PopulateAlgorithmParams(AlgorithmChoice.SelectedIndex);
        }


        private void SetParametersButtonClick(object sender, RoutedEventArgs e)
        {
            if (runThread != null && runThread.IsAlive) return;
            Dictionary<string, int> parameters = new Dictionary<string, int>();
            foreach (StackPanel stackPanel in AlgorithmParamsPanel.Children)
            {
                string name = stackPanel.Children.OfType<Label>().First().Content.ToString();
                int value = int.Parse(stackPanel.Children.OfType<TextBox>().First().Text);
                parameters[name] = value;
            }
            manager.setAlgorithmParams(AlgorithmChoice.SelectedIndex, parameters);
        }

        private void StopButton_Click(object sender, RoutedEventArgs e)
        {
            manager.cancelAlgorithm();
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            manager.cancelAlgorithm();
        }
    }
}