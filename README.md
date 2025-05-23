# Group7

## Correctly pull

1. git pull
2. git submodule update --init --recursive



 2706  ls
 2707  git clone https://github.com/Iaco33/Group7-CAOS.git
 2708  ls
 2709  cd Group7-CAOS
 2710  ls
 2711  git add .
 2712  git commit -m "added nxp_soc and nxp_discovery"
 2713  git add .
 2714  git commit -m "added nxp_soc and nxp_discovery"
 2715  ls
 2716  cd qemu
 2717  git add .
 2718  git commit -m "added nxp_soc and nxp_discovery"
 2719  cd ..
 2720  git add .
 2721  git commit -m "modified qemu, waiting for spi and lpuart"
 2722  git push origin main
 2723  git push
 2724  git pull
 2725  git config pull.rebase true
 2726  git pull
 2727  git push
 2728  LS
 2729  ls
 2730  git add .
 2731  cd Project
 2732  ls
 2733  cd Group7
 2734  LS
 2735  ls
 2736  cd qemu
 2737  ls
 2738  rm -rf .git
 2739  cd ..
 2740  ls
 2741  ls -a
 2742  rm .gitmodules
 2743  git status
 2744  git restore .gitmodules
 2745  ls
 2746  ls -a
 2747  cat .gitmodules
 2748  vim .git/config
 2749  vim .gitmodules
 2750  git status
 2751  git rm --cached qemu
 2752  git add .gitmodules
 2753  git rm --cached qemu
 2754  ls
 2755  cd qemu
 2756  ls
 2757  cd ..
 2758  git status
 2759  git commit -m "Removed submodules + no fork on qemu"
 2760  git push
 2761  git add qemu
 2762  git commit -m "Added qemu without fork"
 2763  git push
 2764  git log
 2765  git push
 2766  ls
 2767  git sta
 2768  git status
 2769  ls
 2770  cd ..
 2771  ls
 2772  cd Group7
 2773  sl
 2774  ls
 2775  cd qemu
 2776  ls
 2777  cd hw/arm
 2778  l
 2779  vi meson.build
 2780  cd ..
 2781  cd char
 2782  ls
 2783  vim meson.build
 2784  ls
 2785  cd ..
 2786  ls
 2787  cd ssi
 2788  ls
 2789  vi meson.build
 2790  cd ..
 2791  cd arm
 2792  ls
 2793  vim Kconfig
 2794  ls
 2795  cd ..
 2796  cd ssi
 2797  vim Kconfig
 2798  ls
 2799  cd ..
 2800  cd char
 2801  ls
 2802  vim Kconfig
 2803  cd ..
 2804  ls
 2805  cd .
 2806  cd ..
 2807  ls
 2808  git pull
 2809  git add .
 2810  git commit -m "modified meson.build+kconfig for ssi,char and arm"
 2811  git push
 2812  git pull
 2813  git push
 2814  git status
 2815  git pull
 2816  ls
 2817  cd qemu
 2818  ls
 2819  build
 2820  ls
 2821  ../configure --target-list=arm-softmmu --enable-debug
 2822  cat /home/vitocnl/Project/Group7/qemu/build/meson-logs/meson-log.txt
 2823  code /home/vitocnl/Project/Group7/qemu/build/meson-logs/meson-log.txt
 2824  ls
 2825  cd ..
 2826  ls
 2827  cd hw/ssi
 2828  ls
 2829  vi Kconfig
 2830  cat Kconfig
 2831  cd ..
 2832  cd build
 2833  ../configure --target-list=arm-softmmu --enable-debug
 2834  code /home/vitocnl/Project/Group7/qemu/build/meson-logs/meson-log.txt
 2835  cd ..
 2836  cd hw/ssi
 2837  ls
 2838  vi Kconfig
 2839  ll
 2840  la
 2841  rm .Kconfig.swp
 2842  vi Kconfig
 2843  cd ..
 2844  ls
 2845  cd arm
 2846  l
 2847  vi Kconfig
 2848  cd ..
 2849  cd ssi
 2850  ls
 2851  vi Kconfig
 2852  cd ..
 2853  cd build
 2854  ../configure --target-list=arm-softmmu --enable-debug
 2855  cd ..
 2856  cd hw/arm
 2857  ls
 2858  cd ..
 2859  cd ssi
 2860  vi meson.build
 2861  cd ..
 2862  cd build
 2863  ../configure --target-list=arm-softmmu --enable-debug
 2864  cd ..
 2865  cd hw/arm
 2866  ll
 2867  vi meson.build
 2868  cd ..
 2869  cd build
 2870  ../configure --target-list=arm-softmmu --enable-debug
 2871  make -j$(nproc)
 2872  D
 2873  make -j$(nproc)
 2874  ls
 2875  make -j$(nproc)
➜  ~
