using System.Numerics;
using UnityEngine;

public class scale : MonoBehaviour
{
    public float scaleSpeedX = 0.5f;
    public float scaleSpeedY = 0.5f;
    public float scaleSpeedZ = 0.5f;
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        UnityEngine.Vector3 s = transform.localScale;   

        s.x += scaleSpeedX * Time.deltaTime;
        s.y += scaleSpeedY * Time.deltaTime;
        s.z += scaleSpeedZ * Time.deltaTime;

        transform.localScale = s;
    }
}
