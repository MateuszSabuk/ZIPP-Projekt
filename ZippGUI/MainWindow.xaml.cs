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
using System.Collections;

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
        private Stack<string> visualisationTaskNames = new Stack<string>();

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
                populateVisualisation();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Generate error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void populateVisualisation()
        {
            while (visualisationTaskNames.Count > 0)
            {
                string name = visualisationTaskNames.Pop();
                if (FindName(name) is FrameworkElement)
                UnregisterName(name);
            }
            VisualisationStackPanel.Children.Clear();
            for (int stageIndex = 0; stageIndex < machines.Length; stageIndex++)
            {
                StackPanel stageRow = new StackPanel { Orientation = Orientation.Vertical, Margin = new Thickness(0, 5, 0, 5) };

                for (int machineIndex = 0; machineIndex < machines[stageIndex]; machineIndex++)
                {
                    visualisationTaskNames.Push("machineRow_stage" + stageIndex + "_machine" + machineIndex);
                    Canvas machineRow = new Canvas
                    {
                        Name = visualisationTaskNames.Peek(),
                        Height = 15,
                        Background = Brushes.Gray,
                        Margin = new Thickness(5, 2, 0, 0)
                    };
                    RegisterName(machineRow.Name, machineRow);
                    stageRow.Children.Add(machineRow);
                }
                VisualisationStackPanel.Children.Add(stageRow);
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
                Tuple<int, int>[,]? result = null;
                try
                {
                    result = manager.run(algId, machines, taskTimes);
                } catch (Exception ex)
                {
                    if (ex.Message != "canceled")
                    {
                        MessageBox.Show(ex.Message, "Run Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    }
                }
                Dispatcher.Invoke(() =>
                {
                    visualizeSchedule(result);
                    RunButton.IsEnabled = true;
                    StopButton.IsEnabled = false;
                });
            }));
            runThread.Start();
        }

        private void visualizeSchedule(Tuple<int, int>[,]? result)
        {
            if (result == null)
            {
                AnswerText.Text = "no results";
                return;
            }
            AnswerText.Text = answerTuple2string(result);

            for (int stageIndex = 0; stageIndex < result.GetLength(1); stageIndex++)
            {
                for (int i = 0; i < machines[stageIndex]; i++)
                {
                    object node = FindName("machineRow_stage" + stageIndex + "_machine" + i);
                    (node as Canvas).Children.Clear();
                }
            }

                List<SolidColorBrush> colorTable = GenerateEvenlySpacedColorTable(taskTimes.Length);

            for (int taskIndex = 0; taskIndex < result.GetLength(0); taskIndex++)
            {
                for (int stageIndex = 0; stageIndex < result.GetLength(1); stageIndex++)
                {
                    object node = FindName("machineRow_stage" + stageIndex + "_machine" + result[taskIndex, stageIndex].Item1);
                    if (node is Canvas)
                    {
                        Canvas machineRow = node as Canvas;

                        Canvas task = new Canvas
                        {
                            Name = "machine" + result[taskIndex, stageIndex].Item1 + "_stage" + stageIndex,
                            Height = machineRow.Height,
                            Width = taskTimes[taskIndex,stageIndex] * 7,
                            Background = colorTable[taskIndex],
                        };
                        Canvas.SetLeft(task, result[taskIndex, stageIndex].Item2 * 7);
                        task.Children.Add(new TextBlock { Text = taskIndex.ToString() });
                        machineRow.Children.Add(task);
                    }
                }
            }
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
        public static string answerTuple2string(Tuple<int, int>[,]? tuples)
        {
            if (tuples == null) return "no results";
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
                string? name = stackPanel.Children.OfType<Label>().First().Content.ToString();
                name = name == null ? string.Empty : name; // Ensure name is not null
                int value = int.Parse(stackPanel.Children.OfType<TextBox>().First().Text);
                parameters[name] = value;
            }
            manager.setAlgorithmParams(AlgorithmChoice.SelectedIndex, parameters);
        }

        private void StopButton_Click(object sender, RoutedEventArgs e)
        {
            manager.cancelAlgorithm();
        }

        // Make sure all running threads are stopped
        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            manager.cancelAlgorithm();
        }

        List<SolidColorBrush> GenerateEvenlySpacedColorTable(int colorCount)
        {
            List<SolidColorBrush> colorTable = new List<SolidColorBrush>();
            double saturation = 0.7; // Fixed saturation for vivid colors
            double lightness = 0.5;  // Fixed lightness for balanced brightness

            for (int i = 0; i < colorCount; i++)
            {
                double hue = (i * 360.0 / colorCount) % 360; // Evenly spaced hues
                colorTable.Add(new SolidColorBrush(HslToRgb(hue, saturation, lightness)));
            }

            return colorTable;
        }

        // Helper function to convert HSL to RGB
        Color HslToRgb(double hue, double saturation, double lightness)
        {
            double c = (1 - Math.Abs(2 * lightness - 1)) * saturation;
            double x = c * (1 - Math.Abs((hue / 60.0) % 2 - 1));
            double m = lightness - c / 2;
            double r = 0, g = 0, b = 0;

            if (0 <= hue && hue < 60) { r = c; g = x; b = 0; }
            else if (60 <= hue && hue < 120) { r = x; g = c; b = 0; }
            else if (120 <= hue && hue < 180) { r = 0; g = c; b = x; }
            else if (180 <= hue && hue < 240) { r = 0; g = x; b = c; }
            else if (240 <= hue && hue < 300) { r = x; g = 0; b = c; }
            else if (300 <= hue && hue < 360) { r = c; g = 0; b = x; }

            byte red = (byte)((r + m) * 255);
            byte green = (byte)((g + m) * 255);
            byte blue = (byte)((b + m) * 255);

            return Color.FromRgb(red, green, blue);
        }
    }
}