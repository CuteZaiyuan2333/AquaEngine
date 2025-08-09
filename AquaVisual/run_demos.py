#!/usr/bin/env python3
"""
AquaVisual Demo Runner
This script runs various AquaVisual demonstration programs and collects their output.
"""

import subprocess
import os
import sys
from pathlib import Path

class AquaVisualDemoRunner:
    def __init__(self):
        self.build_dir = Path("build")
        self.bin_dir = self.build_dir / "bin" / "Debug"
        self.demos = [
            ("SimpleExample", "Basic functionality test"),
            ("TestAquaVisual", "Comprehensive library test suite"),
            ("SceneDemo", "Advanced scene creation demonstration")
        ]
    
    def check_prerequisites(self):
        """Check if all demo executables exist"""
        print("=== Checking Prerequisites ===")
        
        if not self.bin_dir.exists():
            print(f"❌ Build directory not found: {self.bin_dir}")
            return False
        
        missing_demos = []
        for demo_name, _ in self.demos:
            exe_path = self.bin_dir / f"{demo_name}.exe"
            if exe_path.exists():
                print(f"✅ {demo_name}.exe found")
            else:
                print(f"❌ {demo_name}.exe not found")
                missing_demos.append(demo_name)
        
        if missing_demos:
            print(f"\nMissing demos: {', '.join(missing_demos)}")
            print("Please build the project first using CMake.")
            return False
        
        return True
    
    def run_demo(self, demo_name, description):
        """Run a single demo and capture its output"""
        print(f"\n{'='*60}")
        print(f"Running: {demo_name}")
        print(f"Description: {description}")
        print('='*60)
        
        exe_path = self.bin_dir / f"{demo_name}.exe"
        
        try:
            # Run the demo and capture output
            result = subprocess.run(
                [str(exe_path)],
                capture_output=True,
                text=True,
                timeout=30  # 30 second timeout
            )
            
            if result.returncode == 0:
                print("✅ Demo completed successfully!")
                print("\nOutput:")
                print("-" * 40)
                print(result.stdout)
                if result.stderr:
                    print("Warnings/Errors:")
                    print(result.stderr)
            else:
                print(f"❌ Demo failed with exit code: {result.returncode}")
                if result.stdout:
                    print("Output:")
                    print(result.stdout)
                if result.stderr:
                    print("Error:")
                    print(result.stderr)
            
            return result.returncode == 0
            
        except subprocess.TimeoutExpired:
            print("❌ Demo timed out after 30 seconds")
            return False
        except Exception as e:
            print(f"❌ Error running demo: {e}")
            return False
    
    def run_all_demos(self):
        """Run all available demos"""
        print("🚀 AquaVisual Demo Runner")
        print("This script will run all available AquaVisual demonstration programs.\n")
        
        if not self.check_prerequisites():
            return False
        
        successful_demos = 0
        total_demos = len(self.demos)
        
        for demo_name, description in self.demos:
            if self.run_demo(demo_name, description):
                successful_demos += 1
        
        # Summary
        print(f"\n{'='*60}")
        print("DEMO SUMMARY")
        print('='*60)
        print(f"Total demos: {total_demos}")
        print(f"Successful: {successful_demos}")
        print(f"Failed: {total_demos - successful_demos}")
        
        if successful_demos == total_demos:
            print("🎉 All demos completed successfully!")
            return True
        else:
            print("⚠️  Some demos failed. Check the output above for details.")
            return False
    
    def interactive_mode(self):
        """Run demos interactively"""
        print("🎮 Interactive Demo Mode")
        print("Choose which demo to run:\n")
        
        for i, (demo_name, description) in enumerate(self.demos, 1):
            print(f"{i}. {demo_name} - {description}")
        
        print(f"{len(self.demos) + 1}. Run all demos")
        print("0. Exit")
        
        while True:
            try:
                choice = input(f"\nEnter your choice (0-{len(self.demos) + 1}): ").strip()
                
                if choice == "0":
                    print("Goodbye!")
                    break
                elif choice == str(len(self.demos) + 1):
                    self.run_all_demos()
                    break
                else:
                    choice_idx = int(choice) - 1
                    if 0 <= choice_idx < len(self.demos):
                        demo_name, description = self.demos[choice_idx]
                        self.run_demo(demo_name, description)
                    else:
                        print("Invalid choice. Please try again.")
            except ValueError:
                print("Invalid input. Please enter a number.")
            except KeyboardInterrupt:
                print("\nExiting...")
                break

def main():
    runner = AquaVisualDemoRunner()
    
    if len(sys.argv) > 1:
        if sys.argv[1] == "--all":
            return 0 if runner.run_all_demos() else 1
        elif sys.argv[1] == "--interactive":
            runner.interactive_mode()
            return 0
        else:
            print("Usage:")
            print("  python run_demos.py --all          # Run all demos")
            print("  python run_demos.py --interactive  # Interactive mode")
            print("  python run_demos.py                # Interactive mode (default)")
            return 1
    else:
        # Default to interactive mode
        runner.interactive_mode()
        return 0

if __name__ == "__main__":
    sys.exit(main())