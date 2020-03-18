//패턴3의 쥐어잡는 손을 생성하는 스크립트입니다

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SpawnHands : MonoBehaviour
{
    public GameObject Player;
    public GameObject Rocks;
    public float Spawninterval = 2.0f;
    public int Spawnnums = 10;
    private float Spawncooltime = 0.0f;
    private bool isspawning = false;
    private int spawncount = 0;

    void Start()
    {

        isspawning = false;
    }

    void Update()
    {

        if (spawncount > Spawnnums)
        {
            spawncount = 0;
            Spawncooltime = 0;
            isspawning = false;
        }
        if (isspawning && Spawncooltime < Spawninterval / 2 && spawncount < 10)
        {

            spawncount++;
            Spawncooltime = Spawninterval;

            Instantiate(Rocks, Player.transform.position - Vector3.up * 20 + Vector3.forward * 3 + Vector3.left * 4.7f, Quaternion.identity);
        }

        if (Spawncooltime > 0)
            Spawncooltime -= Time.deltaTime;
    }
    public void setOn()
    {
        spawncount = 0;
        Spawncooltime = 0;
        isspawning = true;
    }
}
